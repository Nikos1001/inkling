
#ifndef INKLIB_DEV_API_H
#define INKLIB_DEV_API_H

#include <inkling/window.h>
#include <inkling/gfx.h>
#include <inkling/log.h>
#include <inkling/input.h>

typedef struct {
    ink_logger* logger;
    
    void (*setWindowTitle)(const char* title);
    f32 (*getWindowAspect)();

    bool (*keyDown)(ink_key key);

    ink_buffer (*makeBuffer)();
    void (*uploadBufferData)(ink_buffer buffer, usize size, void* data);
    void (*dropBuffer)(ink_buffer buffer);

    ink_bindings (*makeBindings)(ink_bindingsDesc desc);
    void (*dropBindings)(ink_bindings* bindings); 

    ink_texture (*makeTexture)(ink_textureDesc desc);
    void (*uploadTextureData)(ink_texture texture, usize w, usize h, void* data);
    void (*dropTexture)(ink_texture texture);

    ink_shader (*makeShader)(const char* vertSrc, const char* fragSrc);
    void (*dropShader)(ink_shader shader); 
    void (*beginPass)(ink_framebuffer* fb, ink_passAction* action);

    void (*applyBindings)(ink_bindings* bindings);
    void (*applyPipeline)(ink_pipeline* pipeline);
    void (*updatePipelineUniforms)(ink_pipeline* pipeline, void* uniforms);
    void (*draw)(usize beginVert, usize count);
} inkapi;

#endif
