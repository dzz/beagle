#include <GLXW/glxw.h>

#include <malloc.h>
#include "primitive.h"
#include "OGL_OBJ.h"
#include "command_message.h"

#define COORDINATE_ATTRIBUTE_INDEX  0
#define UV_ATTRIBUTE_INDEX 1

#define NOT_NORMALIZED 0
#define NO_STRIDE 0
#define NO_POINTER_OFFSET 0

/** BASIC TYPE **/
void _primitive_create_coordinate_primitive(void* _primitive, gfx_float* coordinates, int verts, int vlen ){

    gfx_coordinate_primitive* primitive = (gfx_coordinate_primitive*)_primitive;
    //printf("common init : verts( %i) vlen(%i)\n", verts,vlen);
    primitive->_num_verts = verts;
    primitive->mode = GL_TRIANGLE_FAN;

    //printf("genarray\n");

    glGenVertexArrays(1, &primitive->vert_array);
    //printf("bind\n");
    glBindVertexArray(primitive->vert_array);


    //printf("genbuffer\n");
    //install our coordinates
    glGenBuffers(1, &primitive->vert_buffer);
    //printf("genbind\n");
    glBindBuffer(GL_ARRAY_BUFFER, primitive->vert_buffer);
    //printf("upload\n");
    glBufferData(GL_ARRAY_BUFFER, (verts*vlen) * sizeof(GLfloat), 
            coordinates, 
            GL_STATIC_DRAW);

    //printf("attr index\n");
    glVertexAttribPointer(COORDINATE_ATTRIBUTE_INDEX, 
            vlen, 
            GL_FLOAT, 
            NOT_NORMALIZED, 
            NO_STRIDE, 
            NO_POINTER_OFFSET ); 

    //printf("enable array\n");
    glEnableVertexAttribArray(COORDINATE_ATTRIBUTE_INDEX);
    
    OGL_OBJ("varray",   primitive->vert_array,  OGL_RECV);
    OGL_OBJ("vbuffer",  primitive->vert_buffer, OGL_RECV);

}

void primitive_create_coordinate_primitive(void* _primitive, gfx_float* coordinates, int verts, int vlen ){

    gc_msg m;

    size_t dsize = sizeof(gfx_float)*verts*vlen;
    float* cbuf = NULL;
    cbuf = (float*)malloc(dsize);

    memcpy(cbuf, coordinates,dsize);

    m.cmd = GXC_CREATE_COORDINATE_PRIMITIVE;
    m.pta[0].obj = (void*)_primitive;
    m.mma[0].obj = (void*)cbuf;
    m.pta[1].i = verts;
    m.pta[2].i = vlen;

    GXC_ISSUE(m);
}



void primitive_destroy_coordinate_primitive(void* _primitive){

    gfx_coordinate_primitive* primitive = (gfx_coordinate_primitive*)_primitive;
    glDeleteVertexArrays(1,&primitive->vert_array);
    glDeleteBuffers(1,&primitive->vert_buffer);

    OGL_OBJ("varray",   primitive->vert_array,  OGL_DROP);
    OGL_OBJ("vbuffer",  primitive->vert_buffer, OGL_DROP);

}

unsigned int bound_vert_array = -1;

void primitive_render(void* _primitive) {
    
    gfx_coordinate_primitive* primitive = (gfx_coordinate_primitive*)_primitive;

    if(bound_vert_array!=primitive->vert_array) {
        glBindVertexArray(primitive->vert_array);
        bound_vert_array = primitive->vert_array;
    } 
    
    glDrawArrays(primitive->mode, 0, primitive->_num_verts);
}


#define UV_FLOATS_PER_VERT 2
/** UV COORD TYPE **/
void primitive_create_coordinate_uv_primitive(void* _uv_primitive, gfx_float* coordinates, gfx_float* uvs, int verts, int vlen ){

    gfx_coordinate_uv_primitive* uv_primitive = (gfx_coordinate_uv_primitive*)_uv_primitive;
    //printf("common init\n");
    //initialize basic object
    _primitive_create_coordinate_primitive( 
            (gfx_coordinate_primitive*)uv_primitive, 
            coordinates, verts, vlen);

    //install our UVs
    //printf("gen\n");
    glGenBuffers(1, &uv_primitive->uv_buffer);
    //printf("bind\n");
    glBindBuffer(GL_ARRAY_BUFFER, uv_primitive->uv_buffer);
    //printf("buffer\n");
    glBufferData(GL_ARRAY_BUFFER, (verts*vlen) * sizeof(GLfloat), 
            uvs, GL_STATIC_DRAW);

    //printf("attrib\n");
    glVertexAttribPointer(UV_ATTRIBUTE_INDEX, 
            UV_FLOATS_PER_VERT, 
            GL_FLOAT, 
            NOT_NORMALIZED, 
            NO_STRIDE, 
            NO_POINTER_OFFSET ); 

    //printf("enable idx\n");
    glEnableVertexAttribArray(UV_ATTRIBUTE_INDEX);

    OGL_OBJ("uvbuffer",uv_primitive->uv_buffer,OGL_RECV);
}

void primitive_destroy_coordinate_uv_primitive(void* _uv_primitive) {


    gfx_coordinate_primitive* uv_primitive = (gfx_coordinate_primitive*) _uv_primitive;
    /* TODO: determine if this is required or 
     * if the GL spec indicates that primitive_destroy_coordinate_primitive 
     * is all that is required 
     *
     * notably used in label / text / hwgfx api module
     */

    primitive_destroy_coordinate_primitive((gfx_coordinate_primitive*)
            uv_primitive);

    //glDeleteBuffers(1,&uv_primitive->uv_buffer);

    //OGL_OBJ("uvbuffer",uv_primitive->uv_buffer,OGL_DROP);
}

/** PRIMITIVE TEMPLATES **/

void primitive_create_scrquad_primitive(void* primitive) { 
    //primitive_create_screen_primitive(primitive);
}

void primitive_create_dab_primitive(void* primitive) {
    gfx_float dab_verts[8] = {   -1.0, -1.0 ,   1.0, -1.0 ,   1.0, 1.0 ,   -1.0, 1.0  }; 
    primitive_create_coordinate_primitive(primitive, (gfx_float*)dab_verts, 4, 2);
}

void primitive_create_screen_primitive(void* primitive) { 
    gfx_float context_verts[8] = {   0.0, 0.0 ,   1.0, 0.0 ,   1.0, 1.0 ,   0.0, 1.0  }; 

    primitive_create_coordinate_primitive(primitive, (gfx_float*)context_verts, 4, 2); 
}
