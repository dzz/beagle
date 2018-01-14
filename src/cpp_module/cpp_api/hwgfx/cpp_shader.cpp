#include "./cpp_shader.h"

namespace bgl { namespace shader {

vf_shader::vf_shader( const char* vsrc, const char* fsrc) {
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    create_hwgfx_shader(vsrc, fsrc);
#endif
}

vf_shader::~vf_shader() {
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    destroy_hwgfx_shader();
#endif
}

void vf_shader::str_bind_vec4 (const char* param, float x, float y, float z, float w) {
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    shader_bind_vec4( _shader, param, x, y, z, w );
#endif
}

void vf_shader::str_bind_vec3 (const char* param, float x, float y, float z) {
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    shader_bind_vec3( _shader, param, x, y, z );
#endif
}

void vf_shader::str_bind_vec2 (const char* param, float x, float y) {
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    shader_bind_vec2( _shader, param, x, y );
#endif
}

void vf_shader::str_bind_float (const char* param, float x) {
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    shader_bind_float( _shader, param, x );
#endif
}

void vf_shader::str_bind_floats (const char* param, float* floats, unsigned int len) {
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    shader_bind_floats( _shader, param, floats, len );
#endif
}

void vf_shader::str_bind_int (const char* param, int v) {
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    shader_bind_int( _shader, param, v );
#endif
}

void vf_shader::bind() {
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
    shader_bind( _shader );
#endif
}

void bind_render(vf_shader* shader, std::function<void()> binding_function, std::function<void()> rendering_function) {
    shader->bind();
    binding_function();
    rendering_function();
}
/*
void vf_shader::str_bind_texture_unit (const char* param, unsigned int unit);
#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
#endif
}*/

#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
void vf_shader::create_hwgfx_shader( const char* vsrc, const char* fsrc) {
    _shader = static_cast<gfx_shader*>(malloc(sizeof(gfx_shader)));
    shader_load( _shader, vsrc, fsrc );
}

void vf_shader::destroy_hwgfx_shader() {
    shader_drop( _shader );
}
#endif

}}
