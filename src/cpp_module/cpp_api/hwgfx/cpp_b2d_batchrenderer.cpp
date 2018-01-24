#include "./cpp_b2d_batchrenderer.h"

#include <algorithm>

static const std::vector<float> sprite_base_vertex_pos = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0 };
static const std::vector<float> sprite_base_uv = { 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0 };

namespace bgl {

b2d_batchrenderer::b2d_batchrenderer() :
shader( "shaders/bpp/b2d_v.glsl", "shaders/bpp/b2d_p.glsl" ),
default_texture("terrain/tree_icon.png", true)
{ }

void b2d_batchrenderer::add_sprite( bgl::sprites::b2d_sprite sprite ) {
    pending_sprites.push_back( sprite );
}

void b2d_batchrenderer::commit_pass(bgl::camera& camera, std::vector<bgl::sprites::b2d_sprite>& pass, unsigned int& pass_count ) {
    if(pass.empty()) return;
    if((pass_count+1) > gpu_buffers.size() ) {
        gpu_buffers.push_back(new bgl::primitive::channel_primitive({2,2,2,2,1,2,2,4,4}));  
    }


    const auto& gpu_buffer = gpu_buffers[pass_count];

    //@todo , find some heuristic for default sizing these
    std::vector<float> geometry;
    std::vector<float> uvs;
    std::vector<float> translation_local;
    std::vector<float> scale_local;
    std::vector<float> rotation_local;
    std::vector<float> translation_world;
    std::vector<float> scale_world;
    std::vector<float> filter_color;
    std::vector<float> flash_color;

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
            filter_color.push_back( std::get<0>(sprite.filter_color) );
            filter_color.push_back( std::get<1>(sprite.filter_color) );
            filter_color.push_back( std::get<2>(sprite.filter_color) );
            filter_color.push_back( std::get<3>(sprite.filter_color) );
            flash_color.push_back( std::get<0>(sprite.flash_color) );
            flash_color.push_back( std::get<1>(sprite.flash_color) );
            flash_color.push_back( std::get<2>(sprite.flash_color) );
            flash_color.push_back( std::get<3>(sprite.flash_color) );
        }

    }

    float* data[] = { &geometry[0], &uvs[0], &translation_local[0], &scale_local[0], &rotation_local[0], &translation_world[0], &scale_world[0], &filter_color[0], &flash_color[0] };
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


    //group by z layer
    std::stable_sort( pending_sprites.begin(), pending_sprites.end(), []( const auto& l, const auto& r) -> bool {
        return l.z_index < r.z_index;
    });
    //draw from top to bottom 
    std::stable_sort( pending_sprites.begin(), pending_sprites.end(), []( const auto& l, const auto& r) -> bool {
        return l.translation_world.second < r.translation_world.second;
    });

    //group by texture
    std::stable_sort( pending_sprites.begin(), pending_sprites.end(), []( const auto& l, const auto& r) -> bool {
        return l.texture->get_id() < r.texture->get_id();
    });

    //pending_sprites.clear();
    //return;


    bgl::sprites::b2d_sprite* last = nullptr;
    unsigned int pass_count = 0;
    std::vector< bgl::sprites::b2d_sprite> pass;
    for(auto &sprite: pending_sprites) {
        if(last)  {
            if(last->texture->get_id() != sprite.texture->get_id()) {
                commit_pass(camera, pass, pass_count);
            }
        }
        pass.push_back(sprite);
        last = &sprite;
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
