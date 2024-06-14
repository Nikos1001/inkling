
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
} uniforms;

ink_typeInfo uniformsTypeInfo = {
    .size = sizeof(uniforms),
    .nFields = 1,
    .fields = {
        {"uTrans", offsetof(uniforms, uTrans), &mat4TypeInfo}
    }
};

void reload(gameState* state) {
    ink_dropBuffer(state->verts);
    state->verts = ink_makeBuffer();
    f32 verts[] = {
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };
    ink_uploadBufferData(state->verts, sizeof(verts), verts);

    ink_dropBuffer(state->idxs);
    state->idxs = ink_makeBuffer();
    u32 idxs[] = {
        0, 1, 3,
        1, 2, 3
    };
    ink_uploadBufferData(state->idxs, sizeof(idxs), idxs);

    ink_dropBindings(&state->bindings);
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

    ink_dropShader(state->shader);
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

        out vec4 oColor;

        void main() {
            oColor = vec4(pUv, 0.2f, 1.0f);
        } 
    ));

    state->pipeline = (ink_pipeline){
        .shader = state->shader,
        .uniformTypeInfo = &uniformsTypeInfo
    };
}

void update(f32 dt, gameState* state) {
    state->time += dt;

    ink_setWindowTitle("Hello Daniel!");

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

    mat4 cameraTrans = ink_calcOrthoCamMatrix(ink_getWindowAspect(), 5.0, -1.0, 1.0, state->camPos);
    mat4 objTrans = ink_calc2DTransform((vec2){3.0, 2.0}, (vec2){2.0, 2.0}, state->time);

    uniforms u;
    u.uTrans = ink_mulMat4(&objTrans, &cameraTrans); 
    ink_updatePipelineUniforms(&state->pipeline, &u);

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

    ink_draw(0, 6);
}
