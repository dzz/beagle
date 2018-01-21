#include "CPPGFXTest.h"
#include <malloc.h>

#include <utility>

CPPGFXTest::CPPGFXTest():
    shader("shaders/test/vert_chan.glsl","shaders/test/pixel_chan.glsl"), 
    primitive( { 2, 2, 4 } ),
    time ( 0.0f ),
    terrain( 256 ),
    tree_texture("terrain/tree_icon.png", true),
    camera(),
    sprite_renderer()
{

	std::vector<float> verts = {
			-1.0f, -1.0f,
			1.0f,-1.0f,
			1.0f, 1.0f
	};

	std::vector<float> uvs = {
			0.0f, 0.0f,
			1.0f,0.0f,
			1.0f,1.0f
	};

	std::vector<float> colors = 
	{
			1.0f,0.0f,0.0f,1.0f,
			0.0f,1.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,1.0f
	};

	float* data[] = { &verts[0],&uvs[0],&colors[0] };
        primitive.prepare(data,3);

}

CPPGFXTest::~CPPGFXTest() {
    shader.destroy();
    primitive.destroy();
}

void CPPGFXTest::tick() {
    time += 0.006f;    
    printf("trying to set %f on camera\n", time );
    camera.update(std::pair<float, float>(0.0f, 0.0f), 1.0f/time );
}

void CPPGFXTest::render() {

	std::vector<float> verts = {
			-1.0f, -1.0f,
			1.0f,-1.0f,
			1.0f, 1.0f
	};

	std::vector<float> uvs = {
			0.0f, 0.0f,
			1.0f,0.0f,
			1.0f,1.0f
	};

	std::vector<float> colors = 
	{
			1.0f,0.0f,0.0f,1.0f,
			0.0f,1.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,1.0f
	};

	float* data[] = { &verts[0],&uvs[0],&colors[0] };
        primitive.prepare(data,3);

    bgl::shader::bind_render( &shader, [this](){
        /*** bind your shader values here ***/
        this->shader.str_bind_float("u_time", this->time);
        this->camera.bind(shader);
    },
    [this](){
        /*** render your geometry here ***/
        this->primitive.render();
    });

    terrain.render(camera);

    float rotation = 0.0f;
    //float rotation = this->time;

    for(int i=0; i<1000;++i) {
        sprite_renderer.add_sprite({0.0, { 0.0+i,0.0}, { 1.0, 1.0 }, rotation , { 0.0, -10.0}, { 1.0,1.0 }, { 1.0,0.0,1.0,1.0}, {1.0,1.0,1.0,1.0}, &tree_texture });
        sprite_renderer.add_sprite({0.0, { 0.0,0.0+i}, { 1.0, 1.0 }, rotation , { -10.0, 0.0}, { 1.0,1.0 }, { 1.0,0.0,1.0,1.0}, {0.0,0.0,0.0,0.0}, &tree_texture });
        sprite_renderer.add_sprite({0.0, { 0.0,0.0-i}, { 1.0, 1.0 }, rotation , { 10.0, 0.0},  { 1.0,1.0 }, { 1.0,0.0,1.0,1.0}, {0.0,0.0,0.0,0.0}, &tree_texture });
        sprite_renderer.add_sprite({0.0, { 0.0-i,0.0}, { 1.0, 1.0 }, rotation , { 0.0, 10.0},  { 1.0,1.0 }, { 1.0,0.0,1.0,1.0}, {0.0,0.0,0.0,0.0}, &tree_texture });
    }

    bgl::blendmode::use( BLENDMODE_OVER, [&]() {
        sprite_renderer.render(camera); 
    });
}
