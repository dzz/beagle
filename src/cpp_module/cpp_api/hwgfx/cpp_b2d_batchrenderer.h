#ifndef BGLPP_API_HWGFX_B2D_BATCHRENDERER_H
#define BGLPP_API_HWGFX_B2D_BATCHRENDERER_H

#include <utility>
#include "./cpp_channel_primitive.h"
#include "./cpp_shader.h"
#include "./cpp_view.h"
#include "./cpp_camera.h"
#include "./cpp_texture.h"

namespace bgl {

namespace sprites {
    struct b2d_sprite {
        /*float z_index;
        std::pair< float, float> position;
        std::pair< float, float> scale;
        std::tuple< float, float, float, float> filter_color;
        std::tuple< float, float, float, float> flash_color;
        float radians;*/
        bgl::texture* texture;
        
    };
}

class b2d_batchrenderer {
    public:
        b2d_batchrenderer();
        void add_sprite( bgl::sprites::b2d_sprite sprite );
        void render();
        void destroy();
    private:

        void commit_pass( std::vector<bgl::sprites::b2d_sprite>& pass, unsigned int& pass_count );
        std::vector< bgl::sprites::b2d_sprite> pending_sprites;
        std::vector< bgl::primitive::channel_primitive*> gpu_buffers;
        bgl::shader::vf_shader shader; 
};

}

#endif
