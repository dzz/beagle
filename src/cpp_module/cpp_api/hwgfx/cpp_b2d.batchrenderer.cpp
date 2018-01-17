#include "./cpp_b2d_batchrenderer.h"

#include <algorithm>

static const std::vector<float> sprite_base_vertex_pos = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0 };
static const std::vector<float> sprite_base_uv = { 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0 };

namespace bgl {

b2d_batchrenderer::b2d_batchrenderer() { }

void b2d_batchrenderer::add_sprite( bgl::sprites::b2d_sprite sprite ) {
    pending_sprites.push_back( sprite );
}

void b2d_batchrenderer::commit_pass(std::vector<bgl::sprites::b2d_sprite>& pass, unsigned int& pass_count ) {
    return;
    if(pass.empty()) return;
    if((pass_count+1) > gpu_buffers.size() ) {
        gpu_buffers.push_back(new bgl::primitive::channel_primitive({2,2,4}));  
    }
    auto gpu_buffer = gpu_buffers[pass_count];

    std::vector<float> geometry;
    std::vector<float> uvs;

    for(auto sprite: pass) {
        geometry.insert( geometry.end(), sprite_base_vertex_pos.begin(), sprite_base_vertex_pos.end() );
        uvs.insert( uvs.end(), sprite_base_uv.begin(), sprite_base_uv.end() );
    }

    float* data[] = { &geometry[0], &uvs[0]};
    unsigned int vertcount = pass.size()*6;

    //gpu_buffer->prepare( data, vertcount );
 
    pass.clear();
    pass_count++;
}

void b2d_batchrenderer::render() {
    return;
    std::sort( pending_sprites.begin(), pending_sprites.end(), []( const auto& l, const auto& r) -> bool {
        return l.texture->get_id() < r.texture->get_id();
    });
    bgl::sprites::b2d_sprite* last = nullptr;
    unsigned int pass_count = 0;
    std::vector< bgl::sprites::b2d_sprite> pass;
    for(auto sprite: pending_sprites) {
        if(last)  {
            if(last->texture->get_id() != sprite.texture->get_id()) {
                commit_pass(pass, pass_count);
            }
            last = &sprite;
        }
        pass.push_back(sprite);
    }
    commit_pass(pass, pass_count);
    pending_sprites.clear();
}

void b2d_batchrenderer::destroy() {
    for( auto buffer: gpu_buffers ) {
        buffer->destroy();
        delete buffer;
    }
}

}
