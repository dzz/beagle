#include <SDL.h>
#include <GLXW/glxw.h>
#include "../system/log.h"
#include "texture.h"
#include "OGL_OBJ.h"
#include "command_message.h"

int slots[1024];
static unsigned int initialized = 0;

unsigned char* _uc_data(int w,int h) {
        unsigned char* texture_data;
        int i;
        int addr=0;

        texture_data = malloc( w*h*4* sizeof(unsigned char));
        for(i=0; i<(w*h);++i) {
            texture_data[addr++]=0;
            texture_data[addr++]=0;
            texture_data[addr++]=0;
            texture_data[addr++]=0;
        }
        return texture_data;
}
float* _fp_data(int w,int h) {
        float* texture_data;
        int i;
        int addr=0;

        texture_data = malloc( w*h*4 * sizeof(float));
        for(i=0; i<(w*h);++i) {
            texture_data[addr++]=0.0f;
            texture_data[addr++]=0.0f;
            texture_data[addr++]=0.0f;
            texture_data[addr++]=0.0f;
        }
        return texture_data;
}

#define _LOD 0 
#define _NOBORDER 0 

void _texture_gen_id(gfx_texture* texture) {
    glGenTextures(1,&texture->texture_id);
    OGL_OBJ("texture", texture->texture_id,OGL_RECV);
}

void _texture_generate(gfx_texture* texture,int w,int h) {
    unsigned char* texture_data = _uc_data(w,h);

    _texture_gen_id(texture);
    glBindTexture(GL_TEXTURE_2D,texture->texture_id);
    glTexImage2D(GL_TEXTURE_2D,_LOD,GL_RGBA,w,h ,_NOBORDER,
                GL_RGBA, GL_UNSIGNED_BYTE,texture_data);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    free(texture_data);
}
void texture_generate(gfx_texture* texture,int w,int h) {

    gc_msg m;

    m.cmd = GXC_TEXTURE_GENERATE;
    m.pta[0].obj = (void*)texture;
    m.pta[1].i  = w;
    m.pta[2].i = h;

    GXC_ISSUE(m);
}

void texture_generate_filtered(gfx_texture* texture,int w,int h) {
    unsigned char* texture_data = _uc_data(w,h);

    _texture_gen_id(texture);
    glBindTexture(GL_TEXTURE_2D,texture->texture_id);
    glTexImage2D(GL_TEXTURE_2D,_LOD,GL_RGBA,w,h ,_NOBORDER,
                GL_RGBA, GL_UNSIGNED_BYTE,texture_data);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    free(texture_data);
}

void texture_generate_fp(gfx_texture* texture,int w,int h ) {
    float* texture_data = _fp_data(w,h);

    _texture_gen_id(texture);
    glBindTexture(GL_TEXTURE_2D,texture->texture_id);
    glTexImage2D(GL_TEXTURE_2D,_LOD,GL_RGBA32F,w,h ,_NOBORDER,
                GL_RGBA, GL_FLOAT,texture_data);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    free(texture_data);
}

void texture_generate_fp_data(gfx_texture* texture,int w,int h, float*texture_data ) {

    _texture_gen_id(texture);
    glBindTexture(GL_TEXTURE_2D,texture->texture_id);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    ///  glTexSubImage2D( GL_TEXTURE_2D, _LOD,
    ///                  0,0,
    ///                  w,h,
    ///                  GL_RGBA32F, GL_FLOAT, texture_data );
    glTexImage2D(GL_TEXTURE_2D,_LOD,GL_RGBA32F,w,h ,_NOBORDER, GL_RGBA, GL_FLOAT,texture_data);

}
void texture_generate_fp_data_filtered(gfx_texture* texture,int w,int h, float*texture_data ) {

    _texture_gen_id(texture);
    glBindTexture(GL_TEXTURE_2D,texture->texture_id);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    ///  glTexSubImage2D( GL_TEXTURE_2D, _LOD,
    ///                  0,0,
    ///                  w,h,
    ///                  GL_RGBA32F, GL_FLOAT, texture_data );
    glTexImage2D(GL_TEXTURE_2D,_LOD,GL_RGBA32F,w,h ,_NOBORDER, GL_RGBA, GL_FLOAT,texture_data);

}

void _texture_from_SDL_surface(gfx_texture* texture, SDL_Surface* formattedSurface) {
    if(formattedSurface!=0) {

        printf("UPLOAAADING\n");
        printf("UPLOAAADING\n");
        printf("UPLOAAADING\n");
        printf("UPLOAAADING\n");
        printf("UPLOAAADING\n");
        printf("UPLOAAADING\n");
        SDL_LockSurface(formattedSurface);
        glBindTexture(GL_TEXTURE_2D,texture->texture_id);
        glTexSubImage2D(    GL_TEXTURE_2D,
                _LOD,
                0,0,
                formattedSurface->w,formattedSurface->h,        
                GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
                (unsigned char*)formattedSurface->pixels);
        SDL_UnlockSurface(formattedSurface);
    }
    SDL_FreeSurface(formattedSurface);
}



void texture_from_SDL_surface(gfx_texture* texture, SDL_Surface* surf) {

    gc_msg m;
    SDL_Surface *formattedSurface = SDL_ConvertSurfaceFormat(surf,SDL_PIXELFORMAT_RGBA8888,0);

    m.cmd = GXC_TEXTURE_FROM_SDL_SURFACE;
    m.mma[0].obj = __structcp(texture,sizeof(surf));
    m.mma[1].obj = formattedSurface;

    GXC_ISSUE(m);
}

void texture_from_SDL_surface_grayscale(gfx_texture* texture, SDL_Surface* surf) {
    SDL_LockSurface(surf);
    glBindTexture(GL_TEXTURE_2D,texture->texture_id);
    glTexSubImage2D(    GL_TEXTURE_2D,
                        _LOD,
                        0,0,
                        surf->w,surf->h ,
                        GL_RED, GL_UNSIGNED_BYTE,
                        (unsigned char*)surf->pixels);
    SDL_UnlockSurface(surf);

}

void texture_drop(gfx_texture* texture) {
    glDeleteTextures(1,&texture->texture_id);
    OGL_OBJ("texture", texture->texture_id,OGL_DROP);
}

void texture_bind(gfx_texture* texture, int texture_unit) {

    if(!initialized) {
        for(int i=0; i<1024;++i) {
            slots[i]=-1;
        }
    }

    if(slots[texture_unit]!=texture->texture_id) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, texture->texture_id );
        texture->bound_unit = texture_unit;
        slots[texture_unit] = texture->texture_id;
    }
}

void texture_download(gfx_texture* texture, SDL_Surface* target) {
    texture_bind(texture, TEX_UNIT_0);
    SDL_LockSurface(target);
    glGetTexImage(GL_TEXTURE_2D, _LOD, GL_RGBA, GL_UNSIGNED_BYTE, target->pixels );
    SDL_UnlockSurface(target);
}

