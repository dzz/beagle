#ifndef __POINTLIGHT__
#define __POINTLIGHT__

#include "framebuffer.h"

typedef struct {
    float x;
    float y;
    float r;
    float g;
    float b;
    float radius;
    int flare_style;
    int shadow_style;
} gfx_pointlight;

typedef struct{
    float *encoded_lines;
    int num_lines;
    gfx_pointlight* lights;
    int num_lights;
	float view_x;
	float view_y;
	float camera_x;
	float camera_y;
	float camera_scale;
	gfx_framebuffer* composite_target;
    int composite_target_w;
    int composite_target_h;
} gfx_pointlight_context;


void pl_project( float radius, float X,float Y, float*pX, float*pY);
void initPointLights();
void dropPointLights();
void testVolumeCalculate();
void testBaselightRender();
void testRenderLight();
void testRenderLights();


#endif

