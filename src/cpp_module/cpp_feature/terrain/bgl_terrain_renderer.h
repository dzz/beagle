#ifndef BEAGLE_TERRAIN_RENDERER_H
#define BEAGLE_TERRAIN_RENDERER_H

#include "bgl_terrain.h"
#include "../cpp_api/hwgfx/cpp_channel_primitive.h"

namespace bgl {
    class TerrainRenderer {
        public:
            TerrainRenderer( Terrain* model);
            ~TerrainRenderer();
            void render();
        private:
            Terrain* model;
    };
};
#endif
