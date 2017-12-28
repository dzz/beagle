#include <malloc.h>
#include "command_message.h"
#include <math.h>
#include "pointlight.h"
#include "primitive.h"
#include "shader.h"
#include "texture.h"
#include "framebuffer.h"

#include <SDL.h>
#include <SDL_image.h>

// max resolution of an individual shadow map (one light)
#define POINTLIGHT_TEXSIZE 256

//16 megs buffer. This should be big enough for a 1024x1024 map with _every_ tile
//having a shadow occuder. which would be fucking stupid.

#define LINES_BUFSIZE 4194304 

static gfx_shader* baselight_shader;
static gfx_shader* volume_shader;
static gfx_channel_primitive* gpu_volumebuffer;
static gfx_coordinate_uv_primitive* baselight_primitive;

static gfx_texture* compositing_texture;
static gfx_framebuffer* compositing_buffer;

static gfx_texture* pointlight_texture;

static gfx_float** volume_channels;
static int* volume_channel_lens;

float* GLB_volume_tris;

void createLocalVolumeBuffers() {
    volume_channels = malloc(sizeof(gfx_float*)*1);
    volume_channel_lens = malloc(sizeof(int*)*1);
    GLB_volume_tris = malloc(sizeof(gfx_float)*LINES_BUFSIZE);
}

void createBaselightPrimitive() {

    float p[12] = {
        -1.0,-1.0,
        1.0,-1.0,
        1.0,1.0,
        1.0,1.0,
        -1.0,1.0,
        -1.0,-1.0
    };

    float u[12] = {
        0.0,0.0,
        1.0,0.0,
        1.0,1.0,
        1.0,1.0,
        0.0,1.0,
        0.0,0.0
    };

    baselight_primitive = malloc(sizeof(gfx_coordinate_uv_primitive));
    primitive_create_coordinate_uv_primitive( baselight_primitive, p, u, 6, 2 );

}

void createVolumeShader() {
    volume_shader = malloc(sizeof(gfx_shader));
    shader_load(volume_shader, "shaders/pointlight/volume_v.glsl", "shaders/pointlight/volume_p.glsl"); 
}

void createBaselightShader() {
    baselight_shader = malloc(sizeof(gfx_shader));
    shader_load(baselight_shader, "shaders/pointlight/baselight_v.glsl", "shaders/pointlight/baselight_p.glsl"); 
}

void initPointLights() {

    SDL_Surface* light;

    gpu_volumebuffer = malloc(sizeof(gfx_channel_primitive));
    primitive_create_channel_primitive(gpu_volumebuffer, 1,0,0,0);

    compositing_texture = malloc(sizeof(gfx_texture));
    compositing_buffer = malloc(sizeof(gfx_framebuffer));

    texture_generate( compositing_texture, POINTLIGHT_TEXSIZE, POINTLIGHT_TEXSIZE );
    framebuffer_create_framebuffer( compositing_buffer );
    framebuffer_bind_texture( compositing_buffer, compositing_texture );

	light = IMG_Load("light\\radial.png");

    // load the pointlight texture
    pointlight_texture = malloc(sizeof(gfx_texture));
    texture_generate( pointlight_texture, light->w, light->h );
    texture_from_SDL_surface( pointlight_texture, light );
    SDL_FreeSurface(light);

    createBaselightPrimitive();
    createBaselightShader();
    createVolumeShader();

    createLocalVolumeBuffers();
}

void dropPointLights() {
    primitive_destroy_channel_primitive(gpu_volumebuffer);
    primitive_destroy_coordinate_uv_primitive(baselight_primitive);
    texture_drop( pointlight_texture );
    texture_drop( compositing_texture );
    framebuffer_drop( compositing_buffer );
    shader_drop(baselight_shader);
    shader_drop(volume_shader);
    free(volume_channels);
    free(volume_channel_lens);
}





/***


C...........D
 \         /
  \       /
   A.....B
    \   /
     \ /
      O(lx,ly)

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

void fill_volumes( float* volume_tris, float* lines, int num_lines, float radius, float lx, float ly ) {


    //okay... the reduction / transformation could(should) be done in the shader as an optimization
    float reduction = 1.0 / radius;
    for(int i=0; i<num_lines;++i) {

        float Cx,Cy,Dx,Dy;
        float* occluder = &lines[i*4];
        float Ax = (occluder[0] -lx) * reduction;
        float Ay = (occluder[1] -ly) * reduction;
        float Bx = (occluder[2] -lx) * reduction;
        float By = (occluder[3] -ly) * reduction;
        {
            //yes....9000.1f. It's an easier number to work with than infinity.
            pl_project(radius*9000.1f, Ax,Ay, &Cx,&Cy);
            pl_project(radius*9000.1f, Bx,By, &Dx,&Dy);
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

void renderLight( gfx_pointlight* light, gfx_pointlight_context* context) {

    fill_volumes(GLB_volume_tris, context->encoded_lines, context->num_lines, light->radius, light->x, light->y);



   // framebuffer_render_start(compositing_buffer);
    {
        {
            //render the baselight texture (approximate vis of hypot function) 
            shader_bind(baselight_shader);
            texture_bind(pointlight_texture, 0);
            shader_bind_texture(baselight_shader, "light_texture", pointlight_texture);
            primitive_render(baselight_primitive);
        }
        {
            //render the shadow volume
            shader_bind(volume_shader);
            volume_channels[0] = GLB_volume_tris;
            volume_channel_lens[0] = 2;

            primitive_update_channel_primitive_unmanaged(gpu_volumebuffer, volume_channels, volume_channel_lens, context->num_lines * 6 );
            primitive_render( gpu_volumebuffer );
        }
    }
    //framebuffer_render_end(compositing_buffer);
}

static float time = 0.0f;


void testRenderLight() {


	float test_lines[16] = {
		-0.25,-0.25,0.25,-0.25,
		-0.25,0.25,0.25,0.25,
		-0.25,-0.2,-0.25,0.2,
		0.25,-0.2,0.25,0.2
	};
    gfx_pointlight pl;
    gfx_pointlight_context ctx;

    pl.x = 0.0f;
    pl.y = 0.0f;
    pl.radius = 2.0f;

    ctx.encoded_lines = test_lines;
    ctx.num_lines = 4;

    renderLight( &pl, &ctx); 

}

void testBaselightRender() {
        shader_bind(baselight_shader);
        texture_bind(pointlight_texture, 0);
		shader_bind_texture(baselight_shader, "light_texture", pointlight_texture);
        primitive_render(baselight_primitive);
}

void testVolumeCalculate() {

	float test_lines[16] = {
		-0.25,-0.25,0.25,-0.25,
		-0.25,0.25,0.25,0.25,
		-0.25,-0.2,-0.25,0.2,
		0.25,-0.2,0.25,0.2
	};
    float* volume_tris = malloc(4*6*2*sizeof(float));

    float rad = 1.0 + (sin(time*5)*0.5);
    time += 0.01f;
    fill_volumes(volume_tris, test_lines, 4, rad, sin(time)*0.01f, cos(time*2)*0.01f);

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

