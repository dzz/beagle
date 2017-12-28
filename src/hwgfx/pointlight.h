#ifndef POINTLIGHT
#define POINTLIGHT

typedef struct {
    float x;
    float y;
    float radius;
} gfx_pointlight;

typedef struct{
    float *encoded_lines;
    int num_lines;
} gfx_pointlight_context;

void pl_project( float radius, float X,float Y, float*pX, float*pY);
void initPointLights();
void dropPointLights();
void testVolumeCalculate();
void testBaselightRender();
void testRenderLight();


#endif

