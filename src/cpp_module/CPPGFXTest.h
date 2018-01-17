#include "./cpp_api/bgl.h"
#include "./cpp_feature/terrain/bgl_terrain.h"

#ifndef CPPGFXTEST_H

class CPPGFXTest {
    public:
        CPPGFXTest();
        ~CPPGFXTest();
        
        void render();
        void tick();

        bgl::camera camera;
        bgl::shader::vf_shader shader;
        bgl::primitive::channel_primitive primitive;
        bgl::Terrain terrain;
        bgl::texture tree_texture;
        //bgl::b2d_batchrenderer sprite_renderer;

        float time;
};


#endif
