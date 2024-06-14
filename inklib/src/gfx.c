
#include <inkling/gfx.h>
#include <inkling/marks.h>
#include <inkling/log.h>

#include <GL/glew.h>

typedef struct {
    u32 buffer;
    u32 currStride;
} buffer;

ink_typeInfo bufferTypeInfo = {
    .size = sizeof(buffer),
    .nFields = 0
};

ink_respool buffers = ink_makeRespool(&bufferTypeInfo);

ink_buffer ink_makeBuffer() {
    ink_buffer bufferRef = ink_allocRes(&buffers);
    buffer* buffer = ink_getRes(&buffers, bufferRef);
    glGenBuffers(1, &buffer->buffer);
    return bufferRef;
}

void ink_uploadBufferData(ink_buffer bufferRef, usize size, void* data) {
    buffer* buffer = ink_getRes(&buffers, bufferRef);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void ink_dropBuffer(ink_buffer bufferRef) {
    buffer* buffer = ink_getRes(&buffers, bufferRef); 
    if(buffer) {
        glDeleteBuffers(1, &buffer->buffer);
    }
    ink_dropRes(&buffers, bufferRef);
}

typedef struct {
    u32 program;
    bool valid;
} shader;

ink_typeInfo shaderTypeInfo = {
    .size = sizeof(shader),
    .nFields = 0
};

ink_respool shaders = ink_makeRespool(&shaderTypeInfo);

u32 compileShader(u32 type, const char* src, bool* valid) {
    u32 shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    i32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        ink_error(type == GL_VERTEX_SHADER ? "Failed to compile vertex shader." : "Failed to compile fragment shader.");
        char msg[512];
        glGetShaderInfoLog(shader, 512, NULL, msg);
        ink_error(msg);
        *valid = false;
    }

    return shader;
}

u32 compileProgram(const char* vertSrc, const char* fragSrc, bool* valid) {
    u32 vertShader = compileShader(GL_VERTEX_SHADER, vertSrc, valid); 
    u32 fragShader = compileShader(GL_FRAGMENT_SHADER, fragSrc, valid);

    u32 program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    i32 success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        ink_error("Failed to link shader.");
        char msg[512];
        glGetProgramInfoLog(program, 512, NULL, msg);
        ink_error(msg);
        *valid = false;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
   
    return program;
}

ink_shader ink_makeShader(const char* vertSrc, const char* fragSrc) {
    ink_shader shaderRef = ink_allocRes(&shaders);
    shader* shader = ink_getRes(&shaders, shaderRef);
    shader->valid = true;
    shader->program = compileProgram(vertSrc, fragSrc, &shader->valid);

    return shaderRef;
}

void ink_dropShader(ink_shader shaderRef) {
    shader* shader = ink_getRes(&shaders, shaderRef);
    if(shader) {
        glDeleteProgram(shader->program);
    }
    ink_dropRes(&shaders, shaderRef);
}

ink_bindings ink_makeBindings(ink_bindingsDesc desc) {
    ink_bindings bindings;
    glGenVertexArrays(1, &bindings.vao);

    buffer* boundBuffers[INK_MAX_ATTRIBS];

    for(int i = 0; i < desc.nAttribs; i++) {
        boundBuffers[i] = ink_getRes(&buffers, desc.attribs[i].buffer);
        if(boundBuffers[i] != NULL)
            boundBuffers[i]->currStride = 0;
    }

    usize offsets[INK_MAX_ATTRIBS];
    for(int i = 0; i < desc.nAttribs; i++) {
        if(boundBuffers[i] != NULL) {
            offsets[i] = boundBuffers[i]->currStride;
            boundBuffers[i]->currStride += desc.attribs[i].size * sizeof(f32);
        }
    }

    glBindVertexArray(bindings.vao);
    for(int i = 0; i < desc.nAttribs; i++) {
        if(boundBuffers[i] != NULL) {
            glBindBuffer(GL_ARRAY_BUFFER, boundBuffers[i]->buffer);
            glVertexAttribPointer(i, desc.attribs[i].size, GL_FLOAT, GL_FALSE, desc.attribs[i].size * sizeof(f32), (void*)offsets[i]);
            glEnableVertexAttribArray(i); 
        }
    }

    bindings.desc = desc;
    return bindings;
}

void ink_dropBindings(ink_bindings* bindings) {
    glDeleteVertexArrays(1, &bindings->vao);
}

void ink_applyPipeline(ink_pipeline* pipeline) {
    shader* shader = ink_getRes(&shaders, pipeline->shader);
    if(shader != NULL) {
        glUseProgram(shader->program);
    } else {
        ink_error("Tried to bind deleted shader.");
    }
}

void ink_beginPass(ink_framebuffer* fb, ink_passAction* action) {
    if(fb != NULL) {

    } else {
        if(action->color[0].action == INK_CLEAR_ACTION_CLEAR) {
            glClearColor(action->color[0].color.r, action->color[0].color.g, action->color[0].color.b, action->color->color.a);
            glClear(GL_COLOR_BUFFER_BIT); 
        }
    }

    if(action->depth.action == INK_CLEAR_ACTION_CLEAR) {
        glClearDepthf(action->depth.depth);
        glClear(GL_DEPTH_BUFFER_BIT); 
    }
    if(action->stencil.action == INK_CLEAR_ACTION_CLEAR) {
        glClearStencil(action->stencil.value);
        glClear(GL_STENCIL_BUFFER_BIT); 
    }
}

void ink_applyBindings(ink_bindings* bindings) {
    glBindVertexArray(bindings->vao);
}

void ink_updatePipelineUniforms(ink_pipeline* pipeline, void* uniforms) {
    shader* shader = ink_getRes(&shaders, pipeline->shader);
    if(shader != NULL) {
        glUseProgram(shader->program);
    } else {
        ink_error("Tried to bind deleted shader.");
        return;
    }

    if(pipeline->uniformTypeInfo) {
        for(usize i = 0; i < pipeline->uniformTypeInfo->nFields; i++) {
            ink_field* field = &pipeline->uniformTypeInfo->fields[i];
            if(field->type->gfxUniformType == INK_GFX_UNIFORM_TYPE_NONE)
                continue;
            u32 uniformLoc = glGetUniformLocation(shader->program, field->name);
            void* uniformData = uniforms + field->offset;
            switch(field->type->gfxUniformType) {
                case INK_GFX_UNIFORM_TYPE_NONE: {
                    break;
                }
                case INK_GFX_UNIFORM_TYPE_MAT4: {
                    glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, uniformData);
                    break;
                }
            }
        }
    }
}

void ink_draw(usize beginVert, usize count) {
    glDrawArrays(GL_TRIANGLES, beginVert, count);
}
