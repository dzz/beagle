#include "sprite_renderer.h"
#include "../cpp_module/cpp_api/hwgfx/cpp_b2d_batchrenderer.h"
#include "../cpp_module/cpp_api/hwgfx/cpp_texture.h"

#include <utility>
#include <tuple>
#include <unordered_map>

static bgl::b2d_batchrenderer* renderer;
static bgl::camera* camera;

void initSpriteRenderer() {
    renderer = new bgl::b2d_batchrenderer();
    camera = new bgl::camera();
}

static std::unordered_map<gfx_texture*,bgl::texture> promoted_texture_cache;

void SR_add_sprite( float z_index, float tlx, float tly, float slx, float sly, float rad, float twx, float twy, float swx, float swy, float fr, float fg, float fb, float fa, float fcr, float fcg, float fcb, float fca, gfx_texture* texture )  {

	//bgl::texture b2d_texture(texture);

	promoted_texture_cache.insert({ texture, bgl::texture(texture) });

        
	bgl::sprites::b2d_sprite sprite = {
            z_index,
            { tlx, tly},
            { swx, swy },
            rad,
            {twx, twy},
            {swx, swy},
            {fr,fg,fb,fa},
            {fcr,fcg,fcb,fca},
            &promoted_texture_cache[texture]
	};
	renderer->add_sprite(sprite);
	
}

void SR_render(float view_x, float view_y) {
    camera->set_view(view_x, view_y);
    renderer->render(*camera);
}

void dropSpriteRenderer() {
    delete renderer;
}
