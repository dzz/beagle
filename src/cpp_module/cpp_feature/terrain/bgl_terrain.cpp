#include "bgl_terrain.h"
#include "PerlinNoise/PerlinNoise.hpp"
#include <vector>

namespace bgl {

Terrain::Terrain(int size): 
    size(size),
    gpu_terrain( { 2, 1 } ),
    gpu_shader("shaders/bpp/terrain_v.glsl","shaders/bpp/terrain_p.glsl")
{
    this->data = new bgl_terrain_cell[size*size];
    double step = 0.01;

    siv::PerlinNoise perlinGenerator(123456789);

    for( int x = 0; x< size; ++x) {
        for( int y = 0; y< size; ++y) {
            unsigned int addr = (y*size)+x;
            bgl_terrain_cell* cell = &this->data[addr];
            cell->height = perlinGenerator.octaveNoise( x*step, y*step, 8 );
            cell->index = cell->height;
        }
    }

    buildGeometry();
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

            int it = i-(size/2);
            int jt = j-(size/2);

            verts[vert++] = -1.0f + it; 
            verts[vert++] = 0.0f + jt; 
            heights[height++] = h;

            verts[vert++] = 1.0f + it; 
            verts[vert++] = 0.0f + jt; 
            heights[height++] = h;

            verts[vert++] = 1.0f + it; 
            verts[vert++] = 1.0f + jt; 
            heights[height++] = h;

            verts[vert++] = 0.0f + it; 
            verts[vert++] = 0.0f + jt; 
            heights[height++] = h;

            verts[vert++] = 0.0f + it; 
            verts[vert++] = 1.0f + jt;
            heights[height++] = h;

            verts[vert++] = 1.0f + it; 
            verts[vert++] = 1.0f + jt; 
            heights[height++] = h;
        }
    }


    float* channels[2] = { &verts[0], &heights[0] };
    gpu_terrain.prepare( channels, nverts );

}

void Terrain::render() {

    bgl::shader::bind_render( &gpu_shader, [this](){
        /************/
    },
    [this](){
        /*** render your geometry here ***/
        this->gpu_terrain.render();
    });
}

Terrain::~Terrain() {
    delete [] this->data;
}

}

