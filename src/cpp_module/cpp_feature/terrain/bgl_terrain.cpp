#include "bgl_terrain.h"
#include "PerlinNoise/PerlinNoise.hpp"
#include <vector>

namespace bgl {

static shader::vf_shader* terrain_shader = nullptr;
static unsigned int shader_ref_count = 0;

Terrain::Terrain(int size, float sea_level, float rock_level, float alpine_level ): 
    size(size),
    gpu_terrain( { 2, 1 } ),
    sea_level(sea_level),
    rock_level(rock_level),
    alpine_level(alpine_level)
{

    if (terrain_shader == nullptr) {
        terrain_shader = new shader::vf_shader("shaders/bpp/terrain_v.glsl", "shaders/bpp/terrain_p.glsl");
    }
    shader_ref_count++;
    
    this->data = new bgl_terrain_cell[size*size];
    double step = 0.01;

    siv::PerlinNoise perlinGenerator(123456789);

    for( int x = 0; x< size; ++x) {
        for( int y = 0; y< size; ++y) {
            unsigned int addr = (y*size)+x;
            bgl_terrain_cell* cell = &this->data[addr];
            cell->height = perlinGenerator.octaveNoise( x*step, y*step, 5 );
            cell->index = cell->height;
        }
    }

    buildGeometry();

    textures.emplace_back("terrain/water.png",true);
    textures.emplace_back("terrain/grass.png",true);
    textures.emplace_back("terrain/rock.png",true);
    textures.emplace_back("terrain/alpine.png",true);
}

void Terrain::buildGeometry() {
    int ntris = size*size*2;
    int nverts = ntris*3;
    int nfloats = nverts*2;

    std::vector<float> verts(nfloats);
    std::vector<float> heights(nverts);

    unsigned int vert = 0;
    unsigned int height = 0;

    for(int i=0; i< size; ++i) {
        for(int j=0; j<size; ++j) {
           
            float h = ((float)data[(j*size)+i].height)/2.0f;

            float it = float(i)-float(size)/2.0f;
            float jt = float(j)-float(size)/2.0f;

            verts[vert++] = -0.5f + it; 
            verts[vert++] = -0.5f + jt; 
            heights[height++] = h;

            verts[vert++] = 0.5f + it; 
            verts[vert++] = -0.5f + jt; 
            heights[height++] = h;

            verts[vert++] = 0.5f + it; 
            verts[vert++] = 0.5f + jt; 
            heights[height++] = h;

            verts[vert++] = -0.5f + it; 
            verts[vert++] = -0.5f + jt; 
            heights[height++] = h;

            verts[vert++] = -0.5f + it; 
            verts[vert++] = 0.5f + jt;
            heights[height++] = h;

            verts[vert++] = 0.5f + it; 
            verts[vert++] = 0.5f + jt; 
            heights[height++] = h;
        }
    }


    float* channels[2] = { &verts[0], &heights[0] };
    gpu_terrain.prepare( channels, nverts );

}

void Terrain::render(bgl::camera& camera) {

    bgl::shader::bind_render( terrain_shader, [&](){

        camera.bind( *terrain_shader );
        terrain_shader->str_bind_texture("tex_water", &(this->textures[0]), 1);
        terrain_shader->str_bind_texture("tex_grass", &this->textures[1], 2);
        terrain_shader->str_bind_texture("tex_rock", &this->textures[2], 3);
        terrain_shader->str_bind_texture("tex_alpine", &this->textures[3], 4);
        terrain_shader->str_bind_float("sea_level", this->sea_level );
        terrain_shader->str_bind_float("rock_level", this->rock_level );
        terrain_shader->str_bind_float("alpine_level", this->alpine_level );
    },
    [this](){
        /*** render your geometry here ***/
        this->gpu_terrain.render();
    });
}

Terrain::~Terrain() {
    shader_ref_count--;

    if(shader_ref_count==0) {
        terrain_shader->destroy();
        delete terrain_shader;
        terrain_shader = nullptr;
    }

    delete [] this->data;
    for(auto &texture: textures) {
        texture.destroy();
    }
}

}

