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
        bgl::Terrain terrain;
        bgl::texture tree_texture;
        bgl::texture cursor_texture;
        bgl::b2d_batchrenderer sprite_renderer;
        std::vector<std::pair<float, float>> tree_locations;
        float time;
};


#endif
