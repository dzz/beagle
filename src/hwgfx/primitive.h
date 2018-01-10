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


typedef struct{
    PRIMITIVE_BASE

    int nchans;
    int nverts;
    GLuint* channel_buffers;
} gfx_channel_primitive;


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


///GXC

void _primitive_create_coordinate_primitive (void* primitive, gfx_float* coordinates, int verts, int vlen);
void _primitive_create_coordinate_uv_primitive(void* _uv_primitive, gfx_float* coordinates, gfx_float* uvs, int verts, int vlen );
void _primitive_destroy_coordinate_primitive(void* _primitive);
void _primitive_destroy_coordinate_uv_primitive(void* _primitive);
void _primitive_render (void* primitive);

void _primitive_create_channel_primitive(void* _primitive, int nchans, gfx_float** channels,  int*channel_lens, int verts);
void _primitive_destroy_channel_primitive(void * _primitive);
void primitive_destroy_channel_primitive(void * _primitive);
void primitive_create_channel_primitive(void* _primitive, int nchans, gfx_float** channels,  int*channel_lens, int verts);
void _primitive_update_channel_primitive(void* _primitive, gfx_float** channels,  int*channel_lens, int verts);
void primitive_update_channel_primitive(void* _primitive, gfx_float** channels,  int*channel_lens, int verts);
void primitive_update_channel_primitive_unmanaged(void* _primitive, gfx_float** channels,  int*channel_lens, int verts);

#endif
