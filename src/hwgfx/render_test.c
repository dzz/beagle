#include "primitive.h"
#include "shader.h"
#include "text.h"
#include "blend_control.h"

void hwgfx_render_test() {

    const gfx_float verts[4][2] = {
        { -1.0, -1.0 },
        { 1.0,-1.0},
        { 1.0,1.0},
        { -1.0,1.0},
    };

    double zero = 0.0;
    double one = 1.0;

    const gfx_float uvs[4][2] = {
        { zero,zero },
        { one,zero},
        { one,one},
        { zero,one},
    };
    gfx_coordinate_uv_primitive primitive;
    gfx_shader shader;
   
    primitive_create_coordinate_uv_primitive( &primitive, (gfx_float*)verts, (gfx_float*)uvs, 4, 2);
    shader_load(&shader, "shaders/test/vert.glsl", "shaders/test/pixel.glsl");
    shader_bind(&shader);

    //primitive.mode = GL_TRIANGLE_FAN;

    primitive_render( &primitive );
    primitive_destroy_coordinate_uv_primitive( &primitive );

    shader_drop(&shader);

    manual_blend_enter( BLENDMODE_OVER );
    for(int i=0; i<10;++i) {
        text_render(i*8,i*8,1,1,1, "BEAGLE - TEXT RENDERING");
    }
    manual_blend_exit();
}
