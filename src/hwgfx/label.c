#include <stdio.h>
#include "../system/ctt2.h"
#include "../drawing/drawingSurfaces.h"
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include "context.h"
#include "texture.h"
#include "shader.h"
#include "primitive.h"
#include "label.h"

static SDL_Surface* atlas;
static SDL_Surface* font;
gfx_shader  label_shader;
gfx_texture atlas_texture;

static int _atl_cursor;


typedef struct {
    int     cursor_pos;
    void*   prev;
} used_cursor;

used_cursor* recycle_stack;

static const int ATLAS_SIZE = 1024;
static const int ATLAS_COLS = 4;
static const int CHAR_DIMS = 8;

void initLabels() {
    font        = IMG_Load("font\\cga8.png");
    atlas       = createDrawingSurface(1024,1024);
    _atl_cursor = 0;

    texture_generate(&atlas_texture,ATLAS_SIZE,ATLAS_SIZE);
    shader_load(&label_shader, "shaders/hwgfx/label.vert.glsl",
                               "shaders/hwgfx/label.frag.glsl");

    recycle_stack = 0;
}

void dropLabels() {
    shader_drop(&label_shader);
    SDL_FreeSurface(font);
    SDL_FreeSurface(atlas);
    texture_drop(&atlas_texture);
    
    /*clean up atlas reuse stack*/
    {
        used_cursor* iterator = recycle_stack;
        used_cursor* tmp;

        for(;;) {
            if(iterator == 0)
                break;
            tmp = iterator;
            iterator = tmp->prev;
            free(tmp);
        }
    }
}


typedef struct{
    int x;
    int y;
} _pt;

int _get_atlas_cell_w() {
    return ATLAS_SIZE / ATLAS_COLS;
}

int _get_atlas_cell_h() {
    return CHAR_DIMS;
}

int api_host_get_char_dims() {
    return CHAR_DIMS;
}

int _max_strlen() {
    return _get_atlas_cell_w() / CHAR_DIMS;
}

_pt get_cursor_position(int p) {
    _pt r;
    int a = p % ATLAS_COLS;
    int b = p / ATLAS_COLS;

    r.x = a * (ATLAS_SIZE/ATLAS_COLS);
    r.y = b*CHAR_DIMS;
    return r;
}


void label_render(gfx_label* label, float x, float y,float r,float g, float b) {

    viewport_dims dims = gfx_viewport_get_dims();

    shader_bind(&label_shader);
    shader_bind_vec2(&label_shader,"label_pos",x,y);
    shader_bind_vec2(&label_shader,"scr_size",(float)dims.w,(float)dims.h);
    shader_bind_vec3(&label_shader,"label_col",r,g,b);
    texture_bind(&atlas_texture, TEX_UNIT_0);

    primitive_render((gfx_coordinate_primitive*)label->primitive);
}


int dequeue_cursor_pos() {
    if(recycle_stack ==0) {
        int rval = _atl_cursor;
        _atl_cursor++;
        return rval;
    } else {
        int rval = recycle_stack->cursor_pos;
        used_cursor* tmp = recycle_stack;
        recycle_stack = tmp->prev;
        free(tmp);
        //printf("FREED STACK:%x",tmp);
        return rval;
    }
}


void label_generate(gfx_label* label) {
    float uvw,uvh;
    float uo,vo;
    _pt pt;
    label->texture      = malloc(sizeof(gfx_texture));
    label->primitive    = malloc(sizeof(gfx_coordinate_uv_primitive));
    label->_cursor      = _atl_cursor; 
    _atl_cursor++; 

    pt = get_cursor_position(label->_cursor);

    /*TODO: link to constants */
    uvw = 256.0/1024.0;
    uvh = 8.0/1024.0;
    uo = (float)pt.x / 1024.0f;
    vo = (float)pt.y / 1024.0f;

    {
        const gfx_float verts[4][2] = {
            {0.0, 0.0},
            {256.0,0.0},
            {256.0,8.0},
            {0.0,8.0} };
        const gfx_float uvs[4][2] = {
            {uo,     vo    },
            {uo+uvw, vo    },
            {uo+uvw, vo+uvh},
            {uo,     vo+uvh} };
        primitive_create_coordinate_uv_primitive
            (label->primitive,
             (gfx_float*)verts, (gfx_float*)uvs, 4, 2);
    }
          
}

void label_set_text(gfx_label* label, const char* text) {
    int i;                              
    int l;                                      //hold length of text
    SDL_Surface* tex;                           //buffer for label

    l = strlen(text);               

    if( l > _max_strlen() )
        l = _max_strlen();

    tex = createDrawingSurface(8*l,8);          //8x8 font, create buffer to hold rendered text
    label->w = l*CHAR_DIMS;                             //calc dimensions
    label->h = CHAR_DIMS;

    for( i=0; i<l; ++i) {                       //render each char of text
        int val = (int)text[i]+256;

        int basex = val % 32;                   //calc our basic grid position for the char
        int basey = val / 32;

        SDL_Rect src; 
        SDL_Rect dst;

        src.x = basex*8;  
        src.y = basey*8;
        src.w = 8;        
        src.h = 8;

        dst.x = i*8;      
        dst.y = 0;
        dst.w = 8;        
        dst.h = 8;

        SDL_BlitSurface(font,&src,tex,&dst);
    }

    /* blank out the atlas where our label gonna live*/

    {
        _pt         pt;
        SDL_Rect    r;

        pt = get_cursor_position(label->_cursor );
        r.x = pt.x;
        r.y = pt.y;
        r.w = _get_atlas_cell_w();
        r.h = _get_atlas_cell_h();
        SDL_FillRect( atlas, &r, SDL_MapRGBA( atlas->format, 0,0,0,0 ));
    }

    /* now blit our label onto the atlas */ 
    {
        SDL_Rect sr; SDL_Rect r;
        _pt pt;

        pt = get_cursor_position(label->_cursor );
        /*src*/
        sr.x=0;     
        sr.y=0;
        sr.w=label->w; 
        sr.h=label->h;
        /*dst*/
        r.x = pt.x;     
        r.y = pt.y;
        r.w = label->w; 
        r.h = label->h;

        SDL_BlitSurface(tex,&sr,atlas,&r);
    }

    texture_from_SDL_surface(&atlas_texture, atlas);

    SDL_FreeSurface(tex);
}



void label_drop(gfx_label* label) {
    if(recycle_stack == 0) {
        recycle_stack = malloc(sizeof(used_cursor));
        recycle_stack->cursor_pos = label->_cursor;
        recycle_stack->prev = 0;
    } else {
        used_cursor* next = malloc(sizeof(used_cursor));
        next->prev = (void*)recycle_stack;
        recycle_stack = next;
        recycle_stack->cursor_pos = label->_cursor;
    }

    //printf("C: LABELS: ADDED ATLAS TAG TO DEQUEU: %x\n", recycle_stack);

    texture_drop(label->texture);
    free(label->texture);
    primitive_destroy_coordinate_primitive((gfx_coordinate_primitive*)label->primitive);
    free(label->primitive);

}
