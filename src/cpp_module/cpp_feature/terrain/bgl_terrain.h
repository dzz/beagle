#ifndef BEAGLE_TERRAIN_H
#define BEAGLE_TERRAIN_H

#include "../../cpp_api/bgl.h"
#include <vector>

namespace bgl {
    struct bgl_terrain_cell { 
        double height;
        double index;
	};

    class Terrain {
        public:
            Terrain(int size);
            ~Terrain();
            void render();
    
        private:
            bgl_terrain_cell* data;
            int size;

            bgl::primitive::channel_primitive gpu_terrain;
            bgl::shader::vf_shader gpu_shader;
            std::vector<bgl::texture> textures;
            void buildGeometry();
	};
};

#endif
