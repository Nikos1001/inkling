
#include <inkling/common.h>
#include <inkling/window.h>
#include <inkling/gfx.h>
#include <inkling/input.h>
#include <inkling/math.h>

typedef struct {
    f32 time; 
    vec2 camPos;

    ink_buffer verts;
    ink_buffer idxs;
    ink_bindings bindings;
    ink_shader shader;
    ink_pipeline pipeline;
    ink_texture texture;
} gameState;

ink_typeInfo gameStateTypeInfo = {
    .size = sizeof(gameState),
    .nFields = 1,
    .fields = {
        {"time", offsetof(gameState, time), &f32TypeInfo},
        {"camPos", offsetof(gameState, camPos), &vec2TypeInfo}
    }
};

ink_typeInfo* stateTypeInfo() {
    return &gameStateTypeInfo;
}

typedef struct {
    mat4 uTrans;
    ink_texture uColor;
} uniforms;

ink_typeInfo uniformsTypeInfo = {
    .size = sizeof(uniforms),
    .nFields = 2,
    .fields = {
        {"uTrans", offsetof(uniforms, uTrans), &mat4TypeInfo},
        {"uColor", offsetof(uniforms, uColor), &ink_textureTypeInfo}
    }
};

void init(gameState* state) {
    state->verts = ink_makeBuffer();
    f32 verts[] = {
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };
    ink_uploadBufferData(state->verts, sizeof(verts), verts);

    state->idxs = ink_makeBuffer();
    u32 idxs[] = {
        0, 1, 3,
        1, 2, 3
    };
    ink_uploadBufferData(state->idxs, sizeof(idxs), idxs);

    state->bindings = ink_makeBindings((ink_bindingsDesc){
        .nAttribs = 2,
        .attribs[0] = {
            .size = 3,
            .buffer = state->verts
        },
        .attribs[1] = {
            .size = 2,
            .buffer = state->verts
        },
        .idxs = state->idxs
    });

    state->shader = ink_makeShader("#version 330 core\n" INK_STRINGIFY(
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aUv;

        uniform mat4 uTrans;

        out vec2 pUv;

        void main() {
            gl_Position = uTrans * vec4(aPos.x, aPos.y, aPos.z, 1.0);
            pUv = aUv;
        }
    ), "#version 330 core\n" INK_STRINGIFY(

        in vec2 pUv;

        uniform sampler2D uColor;

        out vec4 oColor;

        void main() {
            oColor = sqrt(texture(uColor, pUv));
        } 
    ));

    state->pipeline = (ink_pipeline){
        .shader = state->shader,
        .uniformTypeInfo = &uniformsTypeInfo
    };

    state->texture = ink_makeTexture((ink_textureDesc){
        .format = INK_TEXTURE_FORMAT_RGBA,
        .horizontalWrap = INK_TEXTURE_WRAP_CLAMP_TO_EDGE,
        .verticalWrap = INK_TEXTURE_WRAP_CLAMP_TO_EDGE,
        .minFilter = INK_TEXTURE_FILTER_NEAREST,
        .magFilter = INK_TEXTURE_FILTER_NEAREST,
    });

    u8 textureData[] = {
        255, 0, 0, 255,
        0, 255, 0, 255,
        0, 0, 255, 255,
        255, 255, 0, 255
    };
    ink_uploadTextureData(state->texture, 2, 2, textureData);

}

void reload(gameState* state) {
    
}

void update(f32 dt, gameState* state) {
    state->time += dt;

    ink_setWindowTitle("Hello Brian!");

    f32 red = sinf(0.5f * state->time) * 0.5f + 0.5f;
    
    ink_passAction action = (ink_passAction){
        .color[0] = {
            .action = INK_CLEAR_ACTION_CLEAR,
            .color = (vec4) {red, 1.0, 1.0, 1.0}
        }
    };

    ink_beginPass(NULL, &action);
    ink_applyBindings(&state->bindings);
    ink_applyPipeline(&state->pipeline);

    uniforms u;
    u.uColor = state->texture;

    mat4 cameraTrans = ink_calcOrthoCamMatrix(ink_getWindowAspect(), 5.0, -1.0, 1.0, state->camPos);

    for(i32 x = 0; x < 100; x += 1) {
        for(i32 y = 0; y < 100; y += 1) {
            mat4 objTrans = ink_calc2DTransform((vec2){x + 1, y}, (vec2){1.0, 1.0}, state->time * 5.0);
            u.uTrans = ink_mulMat4(&objTrans, &cameraTrans); 
            ink_updatePipelineUniforms(&state->pipeline, &u);
            ink_draw(0, 6);
        }
    }

    f32 camSpeed = 10.0f;
    if(ink_keyDown(INK_KEY_A)) {
        state->camPos.x -= dt * camSpeed;
    }
    if(ink_keyDown(INK_KEY_D)) {
        state->camPos.x += dt * camSpeed;
    }
    if(ink_keyDown(INK_KEY_W)) {
        state->camPos.y += dt * camSpeed;
    }
    if(ink_keyDown(INK_KEY_S)) {
        state->camPos.y -= dt * camSpeed;
    }

}
