#ifndef BGLPP_API_HWGFX_SHADER_H
#define BGLPP_API_HWGFX_SHADER_H

#include "./cpp_hwgfx_config.h"
#include "./cpp_texture.h"

#ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
#include "../../../hwgfx/shader.h"
#endif

#include <functional>
#include <string>

namespace bgl {
    namespace shader {

        class vf_shader {
            public:
                vf_shader( const char* vsrc, const char* fsrc );
                ~vf_shader();

                void bind();
                // these are not the most efficient because it needs to hash the param 
                // to find the uniform location. see uniform_map
                void str_bind_vec4 (const char* param, float x, float y, float z, float w);
                void str_bind_vec3 (const char* param, float x, float y, float z);
                void str_bind_vec2 (const char* param, float x, float y);
                void str_bind_float (const char* param, float x);
                void str_bind_floats (const char* param, float* floats, unsigned int len);
                void str_bind_int (const char* param, int v);
                void str_bind_texture (const char* param, texture* texture, unsigned int unit );

            private:
                #ifdef BEAGLE_CPPGFX_BACKEND_HWGFX
                gfx_shader* _shader;
                void create_hwgfx_shader(const char* vsrc, const char* fsrc);
                void destroy_hwgfx_shader();
                #endif
        };
        void bind_render(vf_shader* shader, std::function<void()> binding_function, std::function<void()> rendering_function);
    };
};

#endif
