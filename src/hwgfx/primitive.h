#ifndef __GFX_PRIMITIVE__
#define __GFX_PRIMITIVE__

#include <GLXW/glxw.h>
typedef GLfloat gfx_float;

#define PRIMITIVE_BASE \
    int _num_verts; \
    GLuint vert_array; \
    GLuint vert_buffer; \
    GLenum mode;  

typedef struct {
  PRIMITIVE_BASE
} gfx_coordinate_primitive;

typedef struct{
  PRIMITIVE_BASE
  GLuint uv_buffer;
} gfx_coordinate_uv_primitive;


void primitive_create_coordinate_primitive (void* primitive, gfx_float* coordinates, int verts, int vlen);

void primitive_destroy_coordinate_primitive (void* primitive);

//accepts both coord and uv prims
void primitive_render (void* primitive);
void primitive_create_coordinate_uv_primitive (void* uv_primitive, gfx_float* coordinates, gfx_float* uvs, int verts, int vlen);

void primitive_destroy_coordinate_uv_primitive (void* uv_primitive);

void primitive_render_coordinate_uv_primitive (void* uv_primitive);

/** PRIMITIVE TEMPLATES **/
void primitive_create_scrquad_primitive(void* primitive);
void primitive_create_dab_primitive(void* primitive);
void primitive_create_screen_primitive(void* primitive);

#endif
