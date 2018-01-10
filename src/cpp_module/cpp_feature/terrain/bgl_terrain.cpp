#include "bgl_terrain.h"
#include "PerlinNoise/PerlinNoise.hpp"

namespace bgl {

Terrain::Terrain(int size) {
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
}

Terrain::~Terrain() {
    delete [] this->data;
}

}

