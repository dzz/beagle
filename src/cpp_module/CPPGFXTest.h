#include "./cpp_api/bgl.h"
#include "./cpp_feature/terrain/bgl_terrain.h"

#ifndef CPPGFXTEST_H

class CPPGFXTest {
    public:
        CPPGFXTest();
        ~CPPGFXTest();
        void render();

        bgl::shader::vf_shader shader;
        bgl::primitive::channel_primitive primitive;
        bgl::Terrain terrain;

        float time;
};


#endif
