#include "dirty.h"

int _x,_y,_x2,_y2;
char invalid = 1;

//tl br coords
void invalidateDirty(int x, int y, int x2, int y2) {
    if(invalid == 1) {
        invalid = 0;
        _x = x;
        _y = y;
        _x2 = x2;
        _y2 = y2;
    }
    else {
        if (x < _x) _x = x;
        if (y < _y) _y = y;
        if (x2>_x2) _x2 = x2;
        if (y2>_y2) _y2 = y2;
    }
}

void resetDirty() {
        invalid = 1;
        _x=0;
        _x2=1;
        _y=0;
        _y2=1;
}

SDL_Rect getDirtyRect() {
        SDL_Rect ret;

        ret.x =(unsigned int)_x;
        ret.y =(unsigned int) _y;
        ret.w =(unsigned int) _x2-_x;
        ret.h =(unsigned int) _y2-_y;


        if(ret.x>1920) ret.x = 1920;
        if(ret.y>1080) ret.y = 1080;
        
        if(ret.x<=0) ret.x = 0;
        if(ret.y<=0) ret.y = 0;
        if(ret.w<=1) ret.w = 1;
        if(ret.h<=1) ret.h = 1;

        return ret;
}
