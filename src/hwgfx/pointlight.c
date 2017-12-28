#include <malloc.h>
#include <math.h>
#include "primitive.h"
#include "shader.h"

static gfx_channel_primitive* gpu_volumebuffer;

void initPointLights() {
    gpu_volumebuffer = malloc(sizeof(gfx_channel_primitive));
    primitive_create_channel_primitive(gpu_volumebuffer, 1,0,0,0);
}

void dropPointLights() {
    //primitive_destroy_channel_primitive(gpu_volumebuffer);
}


/***


C...........D
 \         /
  \       /
   A.....B
    \   /
     \ /
      O

need: ACB, BCD

****/

void pl_project(float radius, float X, float Y, float* pX, float*pY) {
    float dx = X;
    float dy = Y;
    float len = sqrtf((dx*dx)+(dy*dy));
    
	if (len == 0) {
		*pX = 0;
		*pY = 0;
		return;
	}
    dx/=len;
    dy/=len;
    
	*pX = dx*radius;
	*pY = dy*radius;

}

void fill_volumes( float* volume_tris, float* lines, int num_lines, float radius ) {
    //float* volume_tris = malloc(num_lines*6*2*sizeof(float));
    for(int i=0; i<num_lines;++i) {

        float Cx,Cy,Dx,Dy;
        float* occluder = &lines[i*4];
        float Ax = occluder[0];
        float Ay = occluder[1];
        float Bx = occluder[2];
        float By = occluder[3];
        {
            pl_project(radius, Ax,Ay, &Cx,&Cy);
            pl_project(radius, Bx,By, &Dx,&Dy);
        }

        float *tris = &volume_tris[i*6*2];

        //ACB
        tris[0] = Ax;
        tris[1] = Ay;
        tris[2] = Cx;
        tris[3] = Cy;
        tris[4] = Bx;
        tris[5] = By;
        //BCD
        tris[6] = Bx;
        tris[7] = By;
        tris[8] = Cx;
        tris[9] = Cy;
        tris[10] = Dx;
        tris[11] = Dy;
    } 
}

static float time = 0.0f;
void testPointLightRender() {

	float test_lines[16] = {
		-0.25,-0.25,0.25,-0.25,
		-0.25,0.25,0.25,0.25,
		-0.25,-0.2,-0.25,0.2,
		0.25,-0.2,0.25,0.2
	};
    float* volume_tris = malloc(4*6*2*sizeof(float));

    float rad = 1.0 + (sin(time)*0.5);
    time += 0.01f;
    fill_volumes(volume_tris, test_lines, 4, rad);

    {

        gfx_shader* shader = malloc(sizeof(gfx_shader));
        //gfx_channel_primitive* primitive = malloc(sizeof(gfx_channel_primitive));
        gfx_float** channels = malloc(sizeof(gfx_float*)*1);
        int*    chan_lens = malloc(sizeof(int)*1);

        channels[0] = volume_tris;
        chan_lens[0] = 2;

        shader_load(shader, "shaders/test/pointlight_vert.glsl", "shaders/test/pointlight_pixel.glsl"); 
        shader_bind(shader);
        primitive_update_channel_primitive( gpu_volumebuffer, channels, chan_lens, 24 );

        primitive_render( gpu_volumebuffer );
        shader_drop(shader);

        
        
    }
}

