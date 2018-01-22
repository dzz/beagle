#include <malloc.h>
#include "command_message.h"
#include <math.h>
#include "pointlight.h"
#include "primitive.h"
#include "shader.h"
#include "texture.h"
#include "framebuffer.h"
#include "context.h"
#include "misc.h"
#include "blend_control.h"

#include <SDL.h>
#include <SDL_image.h>

// max resolution of an individual shadow map (one light)
#define POINTLIGHT_TEXSIZE 1024

//16 megs buffer. This should be big enough for a 1024x1024 map with _every_ tile
//having a shadow occuder. which would be fucking stupid.

//#define LINES_BUFSIZE 4194304 

static gfx_shader* composite_shader;
static gfx_shader* baselight_shader;
static gfx_shader* volume_shader;
static gfx_channel_primitive* gpu_volumebuffer;
static gfx_coordinate_uv_primitive* baselight_primitive;

static gfx_texture* compositing_texture;
static gfx_framebuffer* compositing_buffer;

static gfx_texture* pointlight_texture;

//static gfx_float** volume_channels;
//static int* volume_channel_lens;

float* GLB_volume_tris;

static gfx_texture* texture_styles[POINTLIGHT_MAX_STYLES];

void createLocalVolumeBuffers() {
    //volume_channels = malloc(sizeof(gfx_float*)*1);
    //volume_channel_lens = malloc(sizeof(int*)*1);
    //GLB_volume_tris = malloc(sizeof(gfx_float)*LINES_BUFSIZE);
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

void createCompositeShader() {
    composite_shader = malloc(sizeof(gfx_shader));
    shader_load( composite_shader, "shaders/pointlight/composite_v.glsl", "shaders/pointlight/composite_p.glsl");
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
    primitive_create_channel_primitive(gpu_volumebuffer, 2,0,0,0);

    compositing_texture = malloc(sizeof(gfx_texture));
    compositing_buffer = malloc(sizeof(gfx_framebuffer));

    texture_generate_filtered( compositing_texture, POINTLIGHT_TEXSIZE, POINTLIGHT_TEXSIZE );
    framebuffer_create_framebuffer( compositing_buffer );
    framebuffer_bind_texture( compositing_buffer, compositing_texture );

	light = IMG_Load("light\\radial.png");

    // load the pointlight texture
    pointlight_texture = malloc(sizeof(gfx_texture));
    texture_generate_filtered( pointlight_texture, light->w, light->h );
    texture_from_SDL_surface( pointlight_texture, light );
    SDL_FreeSurface(light);

    createBaselightPrimitive();
    createBaselightShader();
    createVolumeShader();
    createCompositeShader();

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
    shader_drop(composite_shader);
  //  free(volume_channels);
  //  free(volume_channel_lens);
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

void fill_volumes( float* volume_tris, float* volume_uvs, float* lines, int num_lines, float radius, float lx, float ly ) {


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
        float *uvs = &volume_uvs[i*6*2];

        //ACB
        tris[0] = Ax;
        tris[1] = Ay;
        tris[2] = Cx;
        tris[3] = Cy;
        tris[4] = Bx;
        tris[5] = By;

        uvs[0] = -1.0;
        uvs[1] = 0.0;
        uvs[2] = -1.0;
        uvs[3] = 0.0;
        uvs[4] = 1.0;
        uvs[5] = 0.0;

        //BCD
        tris[6] = Bx;
        tris[7] = By;
        tris[8] = Cx;
        tris[9] = Cy;
        tris[10] = Dx;
        tris[11] = Dy;

        uvs[6] = 1.0;
        uvs[7] = 0.0;
        uvs[8] = -1.0;
        uvs[9] = 0.0;
        uvs[10] = 1.0;
        uvs[11] = 0.0;
    } 
}


void compositeLight( gfx_pointlight* light, gfx_pointlight_context* context) {

    shader_bind( composite_shader );
    shader_bind_vec2( composite_shader, "view", context->view_x, context->view_y);
    shader_bind_vec2( composite_shader, "scale_local", light->radius, light->radius );
    shader_bind_vec2( composite_shader, "scale_world", context->camera_scale, context->camera_scale );
    shader_bind_vec2( composite_shader, "translation_local", 0.0f,0.0f);
    shader_bind_vec2( composite_shader, "translation_world", light->x + context->camera_x, light->y + context->camera_y );
    shader_bind_vec3( composite_shader, "color", light->r, light->g, light->b );

    texture_bind( compositing_texture, 1);
    shader_bind_texture( composite_shader, "precomputed_light_texture", compositing_texture );

    manual_blend_enter( BLENDMODE_ADD );
    primitive_render( baselight_primitive );
    manual_blend_exit();

}




void _renderLight( gfx_pointlight* light, gfx_pointlight_context* context) {

    float* volume_tris;
    float* volume_tri_uvs;
    gfx_float** volume_channels;
    int* volume_channel_lens;
    volume_tris = malloc( context->num_lines*6*2*sizeof(float));
    volume_tri_uvs = malloc( context->num_lines*6*2*sizeof(float));
    volume_channels = malloc(sizeof(gfx_float*)*2);
    volume_channel_lens = malloc(sizeof(int*)*2);

    fill_volumes(volume_tris, volume_tri_uvs, context->encoded_lines, context->num_lines, light->radius, light->x, light->y);
    {
        {
            //render the baselight texture (approximate vis of hypot function) 
            shader_bind(baselight_shader);

            if(light->style == -1)
                texture_bind(pointlight_texture, 0);
            else
                texture_bind(texture_styles[light->style],0);
            shader_bind_texture(baselight_shader, "light_texture", pointlight_texture);
            primitive_render(baselight_primitive);
        }
        {
            //render the shadow volume
            shader_bind(volume_shader);
            volume_channels[0] = volume_tris;
            volume_channels[1] = volume_tri_uvs;
            volume_channel_lens[0] = 2;
            volume_channel_lens[1] = 2;

            primitive_update_channel_primitive(gpu_volumebuffer, volume_channels, volume_channel_lens, context->num_lines * 6 );


            manual_blend_enter(BLENDMODE_OVER);
            primitive_render( gpu_volumebuffer );
            manual_blend_exit();
        }
    }
}

void renderLight(gfx_pointlight* light, gfx_pointlight_context* context) {

	root_gfx_size rs;
	viewport_dims dims;

	dims.x = 0;
	dims.y = 0;
	dims.w = POINTLIGHT_TEXSIZE;
	dims.h = POINTLIGHT_TEXSIZE;

	gfx_viewport_set_dims(dims);
	framebuffer_render_start(compositing_buffer);
	_renderLight(light, context);
	framebuffer_render_end(compositing_buffer);

	rs = gfx_get_root_gfx_size();
	dims.w = rs.w;
	dims.h = rs.h;
	gfx_viewport_set_dims(dims);
}


void renderLights(gfx_pointlight_context* context) {

	for (int i = 0; i< context->num_lights; ++i) {
		renderLight(&context->lights[i], context);

		if (context->composite_target) {
			viewport_dims dims;
			root_gfx_size rs;
			dims.x = 0;
			dims.y = 0;
			dims.w = context->composite_target_w;
			dims.h = context->composite_target_h;

			gfx_viewport_set_dims(dims);
			framebuffer_render_start(context->composite_target);
			compositeLight(&(context->lights[i]), context);
			framebuffer_render_end(context->composite_target);
			rs = gfx_get_root_gfx_size();
			dims.w = rs.w;
			dims.h = rs.h;
			gfx_viewport_set_dims(dims);
		}
		else {
			compositeLight(&context->lights[i], context);
		}
	}
}

void pointlight_render_lights(gfx_pointlight_context* context) {
    renderLights(context);
}

static float time = 0.0f;
static float time_rl_test = 0.0f;


void testRenderLight() {
	float test_lines[16] = {
		-0.25,-0.25,0.25,-0.25,
		-0.25,0.25,0.25,0.25,
		-0.25,-0.2,-0.25,0.2,
		0.25,-0.2,0.25,0.2
	};
    gfx_pointlight pl;
    gfx_pointlight_context ctx;

    time_rl_test += 0.1f;
    pl.x = sin(time_rl_test)*0.1f;
    pl.y = cos(time_rl_test)*0.1f;
    pl.radius = 2.0f+(sin(time_rl_test*0.5)*0.2);

    ctx.encoded_lines = test_lines;
    ctx.num_lines = 4;
    
    _renderLight( &pl, &ctx); 

}
static float time_ls_test = 0.0f;
void testRenderLights() {

	float test_lines[16] = {
		-0.25,-0.25,0.25,-0.25,
		-0.25,0.25,0.25,0.25,
		-0.25,-0.25,-0.25,0.25,
		0.25,-0.25,0.25,0.25
	};
    gfx_pointlight* lights = malloc(sizeof(gfx_pointlight)*2);

    time_ls_test += 0.01f;
    gfx_pointlight_context ctx;
    ctx.encoded_lines = test_lines;
    ctx.num_lines = 4;

    ctx.num_lights = 2;

    ctx.lights = lights;
    ctx.lights[0].x = sin(time_ls_test);
    ctx.lights[0].y = cos(time_ls_test);
    ctx.lights[0].r = 1.0;
    ctx.lights[0].g = 0.5;
    ctx.lights[0].b = 0.25;
    ctx.lights[0].radius = 5.0+cos(time_ls_test);
    ctx.lights[0].style = -1;

    ctx.lights[1].x = cos(time_ls_test+1.0);
    ctx.lights[1].y = sin(time_ls_test+2.0);
    ctx.lights[1].r = 0.25;
    ctx.lights[1].g = 0.25;
    ctx.lights[1].b = 1.0;
    ctx.lights[1].radius = 5.0+sin(time_ls_test);
    ctx.lights[1].style = -1;
    

    ctx.view_x = 0.1;
    ctx.view_y = 0.1;
    ctx.camera_x = 0.0;
    ctx.camera_y = 0.0;
    ctx.camera_scale = 1.0;

    ctx.composite_target = 0;

    renderLights(&ctx);

}

void pointlight_register_style( int style, gfx_texture* texture) {
    texture_styles[style] = texture; 
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
    float* volume_uvs = malloc(4*6*2*sizeof(float));

    float rad = 1.0 + (sin(time*5)*0.5);
    time += 0.01f;
    fill_volumes(volume_tris, volume_uvs, test_lines, 4, rad, sin(time)*0.01f, cos(time*2)*0.01f);

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

