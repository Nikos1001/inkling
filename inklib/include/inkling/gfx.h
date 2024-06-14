
#ifndef INKLING_GFX_H
#define INKLING_GFX_H

#include <inkling/types.h>
#include <inkling/math.h>
#include <inkling/respool.h>

#define INK_MAX_ATTRIBS 8
#define INK_MAX_COLOR_ATTACHMENTS 8

// Graphics objects

typedef ink_ref ink_buffer;

ink_buffer ink_makeBuffer();
void ink_uploadBufferData(ink_buffer buffer, usize size, void* data);
void ink_dropBuffer(ink_buffer buffer);



typedef struct {
    usize nAttribs;
    struct {
        ink_buffer buffer;
        usize size;
    } attribs[INK_MAX_ATTRIBS];
} ink_bindingsDesc;

typedef struct {
    u32 vao;
    ink_bindingsDesc desc;
} ink_bindings;

ink_bindings ink_makeBindings(ink_bindingsDesc desc);
void ink_dropBindings(ink_bindings* bindings);



typedef ink_ref ink_shader;

ink_shader ink_makeShader(const char* vertSrc, const char* fragSrc);
void ink_dropShader(ink_shader shader);



typedef struct {
    ink_shader shader;
    ink_typeInfo* uniformTypeInfo;
} ink_pipeline;



typedef struct {

} ink_framebuffer;


// Commands

typedef enum {
    INK_CLEAR_ACTION_KEEP,
    INK_CLEAR_ACTION_CLEAR,
} ink_clearAction;

typedef struct {
    struct {
        ink_clearAction action;
        vec4 color;
    } color[INK_MAX_COLOR_ATTACHMENTS];
    struct {
        ink_clearAction action;
        f32 depth;
    } depth;
    struct {
        ink_clearAction action;
        u8 value;
    } stencil;
} ink_passAction;

void ink_beginPass(ink_framebuffer* fb, ink_passAction* action);
void ink_applyBindings(ink_bindings* bindings);
void ink_applyPipeline(ink_pipeline* pipeline);
void ink_updatePipelineUniforms(ink_pipeline* pipeline, void* uniforms);
void ink_draw(usize beginVert, usize count);

#endif
