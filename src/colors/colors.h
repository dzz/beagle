#ifndef __COLORS__
#define __COLORS__

typedef union { 
        unsigned int packed;
        struct {
                unsigned char a;
                unsigned char b;
                unsigned char g;
                unsigned char r;
        } rgba;
}  uint_rgba_map;

typedef struct {
        double h;
        double s;
        double v;
} hsv;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} cp_color;

cp_color rgb_from_dbl(double r,double g, double b);
cp_color rgb_from_hsv(double h, double s, double v);
void hsv_from_rgb( uint_rgba_map col, double *h, double *s, double *v );

#endif
