#include <string.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_image.h>

#include "context.h"
#include "primitive.h"
#include "shader.h"
#include "texture.h"

#include "text.h"

static const int CHAR_DIMS = 8;
static int      charsPerRow;
static int      charsPerCol;

static gfx_texture font_texture;
static gfx_shader  text_shader;

#define MAX_CH 256
static gfx_coordinate_uv_primitive char_prims[MAX_CH];


static void genPrims() {
    double uvw = 1.0 / (double) charsPerRow;
    double uvh = 1.0 / (double) charsPerCol;
    int i;
    for(i=0; i< MAX_CH; ++i) { 

        int xIdx = i % charsPerRow;
        int yIdx = i / charsPerRow;

        double uo = (double)xIdx / (double)charsPerRow;
        double vo = (double)yIdx / (double)charsPerCol;


        const gfx_float verts[4][2] = {
            {0.0, 0.0},
            {8.0,0.0},
            {8.0,8.0},
            {0.0,8.0} };
        const gfx_float uvs[4][2] = {
            {uo,     vo    },
            {uo+uvw, vo    },
            {uo+uvw, vo+uvh},
            {uo,     vo+uvh} };
        primitive_create_coordinate_uv_primitive
            (&char_prims[i],
             (gfx_float*)verts, (gfx_float*)uvs, 4, 2);
    }
}


void initText() {
    {
        SDL_Surface* font;
        /*load font and texgen*/

        printf("LOAD FONT\n");


        #ifdef _WIN32
        font        = IMG_Load("font\\cga8.png");
        #endif

        #ifdef __linux__
        printf("linux fallback fontloader...\n");
        font        = IMG_Load("./font/cga8.png");
        #endif

        if(font==0) {
            printf("error...\n");
            printf("IMG_Load error: %s\n", IMG_GetError());
        }

        texture_generate( &font_texture, font->w, font->h );
        texture_from_SDL_surface( &font_texture, font);
        charsPerRow = font->w / CHAR_DIMS; 
        charsPerCol = font->h / CHAR_DIMS;

        SDL_FreeSurface(font);
    }

    shader_load(&text_shader, "shaders/hwgfx/text.vert.glsl",
            "shaders/hwgfx/text.frag.glsl");

    genPrims();
}

void renderChar(float x, float y, int v ) {
    shader_bind_vec2(&text_shader, "char_pos", x,y );
    primitive_render( &char_prims[v] );
}

void text_render( float x, float y,float r, float g, float b, const char* text ) {
    int l;
    int i;
    int it;
    float yt = y;

    viewport_dims dims = gfx_viewport_get_dims();

    shader_bind(&text_shader);
    shader_bind_vec2(&text_shader,"scr_size",(float)dims.w,(float)dims.h);
    shader_bind_vec3(&text_shader,"label_col",r,g,b);
    texture_bind(&font_texture, TEX_UNIT_0);
    l = strlen(text);
    it = 0;
    for( i=0; i<l; ++i ) {
        if( text[i] == '\n' ){
            yt += CHAR_DIMS;
            it = 0;
            
            continue;
        } 
        renderChar( x + it*(CHAR_DIMS), yt, (int)text[i] );
        it++;
    }
}

static void delPrims() {
    int i;
    for(i=0; i< MAX_CH; ++i) {
        primitive_destroy_coordinate_primitive( &char_prims[i] );
    }
}

void dropText() {
    texture_drop(&font_texture);
    shader_drop(&text_shader);
    delPrims();
}

