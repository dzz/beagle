#include "bgl_terrain.h"
#include "PerlinNoise/PerlinNoise.hpp"

namespace bgl {

Terrain::Terrain(int size) {
    this->data = new bgl_terrain_cell[size*size];
}

Terrain::~Terrain() {
    delete [] this->data;
}

}

