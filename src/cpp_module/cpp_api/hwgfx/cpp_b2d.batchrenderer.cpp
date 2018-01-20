#include "./cpp_b2d_batchrenderer.h"

#include <algorithm>

static const std::vector<float> sprite_base_vertex_pos = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0 };
static const std::vector<float> sprite_base_uv = { 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0 };

namespace bgl {

b2d_batchrenderer::b2d_batchrenderer() :
shader( "shaders/bpp/b2d_v.glsl", "shaders/bpp/b2d_p.glsl" )
{ }

void b2d_batchrenderer::add_sprite( bgl::sprites::b2d_sprite sprite ) {
    pending_sprites.push_back( sprite );
}

void b2d_batchrenderer::commit_pass(bgl::camera& camera, std::vector<bgl::sprites::b2d_sprite>& pass, unsigned int& pass_count ) {
    if(pass.empty()) return;
    if((pass_count+1) > gpu_buffers.size() ) {
        gpu_buffers.push_back(new bgl::primitive::channel_primitive({2,2,2,2,1,2,2}));  
    }
    const auto& gpu_buffer = gpu_buffers[pass_count];

    std::vector<float> geometry;
    std::vector<float> uvs;
    std::vector<float> translation_local;
    std::vector<float> scale_local;
    std::vector<float> rotation_local;
    std::vector<float> translation_world;
    std::vector<float> scale_world;

    for(auto sprite: pass) {
        geometry.insert( geometry.end(), sprite_base_vertex_pos.begin(), sprite_base_vertex_pos.end() );
        uvs.insert( uvs.end(), sprite_base_uv.begin(), sprite_base_uv.end() );
        for(int i=0; i<6; ++i) {
            translation_local.push_back( sprite.translation_local.first );
            translation_local.push_back( sprite.translation_local.second );
            scale_local.push_back( sprite.scale_local.first );
            scale_local.push_back( sprite.scale_local.second );
            rotation_local.push_back( sprite.rotation_local );
            translation_world.push_back( sprite.translation_world.first );
            translation_world.push_back( sprite.translation_world.second );
            scale_world.push_back( sprite.scale_world.first );
            scale_world.push_back( sprite.scale_world.second );
        }

    }

    float* data[] = { &geometry[0], &uvs[0], &translation_local[0], &scale_local[0], &rotation_local[0], &translation_world[0], &scale_world[0] };
    unsigned int vertcount = pass.size()*6;

    gpu_buffer->prepare( data, vertcount );
    bgl::shader::bind_render( &shader, [&](){ 
        camera.bind( shader );
        shader.str_bind_texture("sprite", pass[0].texture, 0 );
    },[gpu_buffer](){ gpu_buffer->render(); });
 
    pass.clear();
    pass_count++;
}

void b2d_batchrenderer::render(bgl::camera& camera ) {
    std::sort( pending_sprites.begin(), pending_sprites.end(), []( const auto& l, const auto& r) -> bool {
        return l.texture->get_id() < r.texture->get_id();
    });
    bgl::sprites::b2d_sprite* last = nullptr;
    unsigned int pass_count = 0;
    std::vector< bgl::sprites::b2d_sprite> pass;
    for(auto sprite: pending_sprites) {
        if(last)  {
            if(last->texture->get_id() != sprite.texture->get_id()) {
                commit_pass(camera, pass, pass_count);
            }
            last = &sprite;
        }
        pass.push_back(sprite);
    }
    commit_pass(camera, pass, pass_count);
    pending_sprites.clear();
}

void b2d_batchrenderer::destroy() {
    for( auto buffer: gpu_buffers ) {
        buffer->destroy();
        delete buffer;
    }
    shader.destroy();
}

}
