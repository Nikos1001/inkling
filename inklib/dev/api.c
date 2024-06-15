
#include "api.h"

#include <inkling/marks.h>
#include <stdarg.h>
#include <stdio.h>

inkapi api;

void inkapi_init(inkapi extApi) {
    api = extApi;
}

void ink_setWindowTitle(const char* title) {
    api.setWindowTitle(title);
}

f32 ink_getWindowAspect() {
    return api.getWindowAspect();
}

bool ink_keyDown(ink_key key) {
    return api.keyDown(key);
}

bool ink_keyPressed(ink_key key) {
    return api.keyPressed(key);
}

bool ink_keyReleased(ink_key key) {
    return api.keyReleased(key);
}

ink_buffer ink_makeBuffer() {
    return api.makeBuffer();
}

void ink_uploadBufferData(ink_buffer buffer, usize size, void* data) {
    api.uploadBufferData(buffer, size, data);
}

void ink_dropBuffer(ink_buffer buffer) {
    api.dropBuffer(buffer);
}

ink_bindings ink_makeBindings(ink_bindingsDesc desc) {
    return api.makeBindings(desc);
}

void ink_dropBindings(ink_bindings* bindings) {
    api.dropBindings(bindings);
}

ink_texture ink_makeTexture(ink_textureDesc desc) {
    return api.makeTexture(desc);
}

void ink_uploadTextureData(ink_texture texture, usize w, usize h, void* data) {
    api.uploadTextureData(texture, w, h, data);
}

void ink_dropTexture(ink_texture texture) {
    api.dropTexture(texture);
}

ink_shader ink_makeShader(const char* vertSrc, const char* fragSrc) {
    return api.makeShader(vertSrc, fragSrc);
}

void ink_dropShader(ink_shader shader) {
    api.dropShader(shader);
}

void ink_beginPass(ink_framebuffer* fb, ink_passAction* action) {
    api.beginPass(fb, action);
}

void ink_applyBindings(ink_bindings* bindings) {
    api.applyBindings(bindings);
}

void ink_applyPipeline(ink_pipeline* pipeline) {
    api.applyPipeline(pipeline);
}

void ink_updatePipelineUniforms(ink_pipeline* pipeline, void* uniforms) {
    api.updatePipelineUniforms(pipeline, uniforms);
}

void ink_draw(usize beginVert, usize count) {
    api.draw(beginVert, count);
}

ink_threadLocal char logBuffer[4096];

void ink_log(const char* msg, const char* file, u32 line, ink_log_type type, ...) {
    va_list args;
    va_start(args, type);
    vsnprintf(logBuffer, sizeof(logBuffer), msg, args);
    va_end(args);
    api.logger->log(&api.logger, logBuffer, file, line, type);
}
