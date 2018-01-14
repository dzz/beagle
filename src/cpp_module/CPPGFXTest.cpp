#include "CPPGFXTest.h"
#include <malloc.h>

CPPGFXTest::CPPGFXTest():
    shader("shaders/test/vert_chan.glsl","shaders/test/pixel_chan.glsl"), 
    primitive( { 2, 2, 4 } ),
    time ( 0.0f ),
    terrain( 256 ) 
{

	float verts[] = {
			-1.0f, -1.0f,
			1.0f,-1.0f,
			1.0f, 1.0f
	};

	float uvs[] = {

			0.0f, 0.0f,
			1.0f,0.0f,
			1.0f,1.0f
	};

	float colors[] = 
	{
			1.0f,0.0f,0.0f,1.0f,
			0.0f,1.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,1.0f
	};

	float* data[] = { verts,uvs,colors };
        primitive.prepare(data,3);

}

CPPGFXTest::~CPPGFXTest() {
}

void CPPGFXTest::render() {
    time += 0.1f;    

    bgl::shader::bind_render( &shader, [this](){
        /*** bind your shader values here ***/
        this->shader.str_bind_float("u_time", this->time);
    },
    [this](){
        /*** render your geometry here ***/
        this->primitive.render();
    });

  terrain.render();
    
}
