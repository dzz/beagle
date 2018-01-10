#ifndef BEAGLE_TERRAIN_H
#define BEAGLE_TERRAIN_H

namespace bgl {
    struct bgl_terrain_cell { 
        double height;
        double index;
	};

    class Terrain {
        public:
            Terrain(int size);
            ~Terrain();
        private:
            bgl_terrain_cell* data;
	};
};

#endif
