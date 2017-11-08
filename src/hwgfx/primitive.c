
#include "../memory.h"
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

void _primitive_update_channel_primitive(void* _primitive, gfx_float** channels,  int*channel_lens, int verts);
void _primitive_create_channel_primitive(void* _primitive, int nchans, gfx_float** channels,  int*channel_lens, int verts) {

    GLuint VertArray;
    GLuint *Buffers = malloc(nchans*sizeof(GLuint));
    gfx_channel_primitive* prim=(gfx_channel_primitive*)_primitive;

    glGenVertexArrays(1, &VertArray);
    glBindVertexArray( VertArray);
    prim->vert_array = VertArray;
    glGenBuffers( nchans, Buffers );
    prim->channel_buffers = Buffers;
    prim->nchans = nchans;
    prim->_num_verts = verts;

    _primitive_update_channel_primitive( _primitive, channels, channel_lens, verts ); 
    //for(int i=0; i<nchans; ++i) {

    //    GLuint bsize = (GLuint)channel_lens[i]*verts;

    //    glBindBuffer(GL_ARRAY_BUFFER, Buffers[i] );
    //    glBufferData(GL_ARRAY_BUFFER, verts*channel_lens[i] * sizeof(GLfloat), channels[i], GL_STREAM_DRAW );
    //    glVertexAttribPointer((GLuint)i, 
    //            channel_lens[i], 
    //            GL_FLOAT, 
    //            NOT_NORMALIZED, 
    //            NO_STRIDE, 
    //            NO_POINTER_OFFSET ); 
    //    glEnableVertexAttribArray((GLuint)i);
    //}
}

void primitive_create_channel_primitive(void* _primitive, int nchans, gfx_float** channels,  int*channel_lens, int verts) {

    gc_msg m;

    m.cmd = GXC_CREATE_CHANNEL_PRIMITIVE;
    m.pta[0].obj = (void*)_primitive;
    m.mma[0].obj = (void*)channels;
    m.mma[1].obj = (void*)channel_lens;
    m.pta[1].i = nchans;
    m.pta[2].i = verts; 

    GXC_ISSUE(m);
}

void _primitive_update_channel_primitive(void* _primitive, gfx_float** channels,  int*channel_lens, int verts) {


    gfx_channel_primitive* prim=(gfx_channel_primitive*)_primitive;
    GLuint *Buffers = prim->channel_buffers;
    GLuint VertArray = prim->vert_array;

    glBindVertexArray( prim->vert_array );

    for(int i=0; i<prim->nchans; ++i) {

        GLuint bsize = (GLuint)channel_lens[i]*verts;

        glBindBuffer(GL_ARRAY_BUFFER, Buffers[i] );
        glBufferData(GL_ARRAY_BUFFER, verts*channel_lens[i] * sizeof(GLfloat), channels[i], GL_STREAM_DRAW );
        glVertexAttribPointer((GLuint)i, 
                channel_lens[i], 
                GL_FLOAT, 
                NOT_NORMALIZED, 
                NO_STRIDE, 
                NO_POINTER_OFFSET ); 
        glEnableVertexAttribArray((GLuint)i);
    }

    prim->_num_verts = verts;

}

void primitive_update_channel_primitive(void* _primitive, gfx_float** channels,  int*channel_lens, int verts) {

    gc_msg m;

    m.cmd = GXC_UPDATE_CHANNEL_PRIMITIVE;
    m.pta[0].obj = (void*)_primitive;
    m.mma[0].obj = (void*)channels;
    m.mma[1].obj = (void*)channel_lens;
    m.pta[1].i = verts; 

    GXC_ISSUE(m);
}

void _primitive_destroy_channel_primitive(void * _primitive) {
    gfx_channel_primitive* prim = (gfx_channel_primitive*)_primitive;
    glDeleteVertexArrays(1,&(prim->vert_array)); 
    glDeleteBuffers( prim->nchans, prim->channel_buffers);
    free(prim->channel_buffers);
}

void primitive_destroy_channel_primitive(void * _primitive) {

    gc_msg m;

    m.cmd = GXC_DESTROY_CHANNEL_PRIMITIVE;
    m.mma[0].obj = _primitive;

    GXC_ISSUE(m);
}


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

    //printf("Created buffer: %x\n" , primitive->vert_buffer);

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



void _primitive_destroy_coordinate_primitive(void* _primitive){

    gfx_coordinate_primitive* primitive = (gfx_coordinate_primitive*)_primitive;
    glDeleteVertexArrays(1,&(primitive->vert_array));
    glDeleteBuffers(1,&(primitive->vert_buffer));

    OGL_OBJ("varray",   primitive->vert_array,  OGL_DROP);
    OGL_OBJ("vbuffer",  primitive->vert_buffer, OGL_DROP);

}

void primitive_destroy_coordinate_primitive(void* _primitive){

    size_t sz = sizeof(gfx_coordinate_primitive);
    
    gc_msg m;
    m.cmd = GXC_DESTROY_COORDINATE_PRIMITIVE;
    m.mma[0].obj = _primitive;

    GXC_ISSUE(m);
    
}

void _primitive_destroy_coordinate_uv_primitive(void* _primitive){

    gfx_coordinate_uv_primitive* primitive = (gfx_coordinate_uv_primitive*)_primitive;
    glDeleteVertexArrays(1,&(primitive->vert_array));
    //printf("about to delete vert buffer %x\n", primitive->vert_buffer); //hrmm....
    glDeleteBuffers(1,&(primitive->vert_buffer));

    //printf("about to delete uv buffer %x\n", primitive->uv_buffer); //hrmm....
    glDeleteBuffers(1,&(primitive->uv_buffer));

    OGL_OBJ("varray",   primitive->vert_array,  OGL_DROP);
    OGL_OBJ("vbuffer",  primitive->vert_buffer, OGL_DROP);
    OGL_OBJ("uvbuffer",  primitive->uv_buffer, OGL_DROP);

}

void primitive_destroy_coordinate_uv_primitive(void* _primitive){

    gc_msg m;
    m.cmd = GXC_DESTROY_COORDINATE_UV_PRIMITIVE; //aha
    m.mma[0].obj = _primitive;

    GXC_ISSUE(m);
    
}

unsigned int bound_vert_array = -1;

void _primitive_render(void* _primitive) {
    
    gfx_coordinate_primitive* primitive = (gfx_coordinate_primitive*)_primitive;

    if(bound_vert_array!=primitive->vert_array) {
        //printf("c prim gxc render %u\n", primitive->vert_array);
        glBindVertexArray(primitive->vert_array);
        bound_vert_array = primitive->vert_array;
    } 
    
    glDrawArrays(GL_TRIANGLES, 0, primitive->_num_verts);
}

void primitive_render(void* _primitive) {

    gc_msg m;
	m.cmd = GXC_RENDER_PRIMITIVE;
    m.pta[0].obj = _primitive;

    GXC_ISSUE(m);
}


#define UV_FLOATS_PER_VERT 2
/** UV COORD TYPE **/
void _primitive_create_coordinate_uv_primitive(void* _uv_primitive, gfx_float* coordinates, gfx_float* uvs, int verts, int vlen ){

    gfx_coordinate_uv_primitive* uv_primitive = (gfx_coordinate_uv_primitive*)_uv_primitive;
    //printf("common init\n");
    //initialize basic object
    _primitive_create_coordinate_primitive( 
            (gfx_coordinate_primitive*)uv_primitive, 
            coordinates, verts, vlen);

    //install our UVs
    //printf("gen\n");
    glGenBuffers(1, &(uv_primitive->uv_buffer));
    //printf("Created buffer: %x\n" , uv_primitive->uv_buffer);
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

void primitive_create_coordinate_uv_primitive(void* _uv_primitive, gfx_float* coordinates, gfx_float* uvs, int verts, int vlen ){

    gc_msg m;

    size_t dsize = sizeof(gfx_float)*verts*vlen;
    float* cbuf = NULL;
    float* ubuf = NULL;
    cbuf = malloc(dsize);
    ubuf = malloc(dsize);

    memcpy(cbuf, coordinates,dsize);
    memcpy(ubuf, uvs,dsize);

    m.cmd = GXC_CREATE_COORDINATE_UV_PRIMITIVE;
    m.pta[0].obj = (void*)_uv_primitive;
    m.mma[0].obj = (void*)cbuf;
    m.mma[1].obj = (void*)ubuf;
    m.pta[1].i = verts;
    m.pta[2].i = vlen;

    GXC_ISSUE(m); 
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
