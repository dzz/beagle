#include <math.h>

#include "colors.h"


double MIN( double a, double b, double c) {
    double m = a;
    if(m > b) m = b;
    if(m > c) m = c;
    return m;
}

double MAX( double a, double b, double c) {
    double m = a;
    if(m < b) m = b;
    if(m < c) m = c;
    return m;
}

cp_color rgb_from_dbl(double r,double g, double b) {
    cp_color ret;
    ret.r = (unsigned char)(r*255);
    ret.g = (unsigned char)(g*255);
    ret.b = (unsigned char)(b*255);
    ret.a = 255;
    return ret;
}

cp_color rgb_from_hsv(double h, double s, double v)
{
    double c = 0.0, m = 0.0, x = 0.0;
    {
        c = v * s;
        x = c * (1.0 - fabs(fmod(h / 60.0, 2) - 1.0));
        m = v - c;
        if (h >= 0.0 && h < 60.0)
        {
            return rgb_from_dbl(c + m, x + m, m);
        }
        else if (h >= 60.0 && h < 120.0)
        {
            return rgb_from_dbl(x + m, c + m, m);
        }
        else if (h >= 120.0 && h < 180.0)
        {
            return rgb_from_dbl(m, c + m, x + m);
        }
        else if (h >= 180.0 && h < 240.0)
        {
            return rgb_from_dbl(m, x + m, c + m);
        }
        else if (h >= 240.0 && h < 300.0)
        {
            return rgb_from_dbl(x + m, m, c + m);
        }
        else if (h >= 300.0 && h < 360.0)
        {
            return rgb_from_dbl(c + m, m, x + m);
        }
        else
        {
            return rgb_from_dbl(m, m, m);
        }
    }
    return rgb_from_dbl(0,0,0);
}

void _hsv_from_rgb( uint_rgba_map col, double *h, double *s, double *v );
void hsv_from_rgb( uint_rgba_map col, double *h, double *s, double *v ) {

        if(col.packed == 0) {
                *h = 0;
                *s = 0;
                *v = 0;
                return;
        } else {
                int sum = col.rgba.r+col.rgba.g+col.rgba.b;
                if (sum == (255+255+255)){
                        *h = 0;
                        *s = 0;
                        *v = 1;
                        return;
                }
        } 

        _hsv_from_rgb(col,h,s,v);
}

void _hsv_from_rgb( uint_rgba_map col, double *h, double *s, double *v )
{
        double min, max, delta;

        double r = (float)(col.rgba.r)/255.0f;
        double g = (float)(col.rgba.g)/255.0f;
        double b = (float)(col.rgba.b)/255.0f;

        min = MIN( r, g, b );
        max = MAX( r, g, b );
        *v = max; // v
        delta = max - min;
        if( max != 0 )
                *s = delta / max; // s
        else {
                // r = g = b = 0 // s = 0, v is undefined
                *s = 0;
                *h = -1;
                *v = 0;
                return;
        }
        if( r == max )
                *h = ( g - b ) / delta; // between yellow & magenta
        else if( g == max )
                *h = 2 + ( b - r ) / delta; // between cyan & yellow
        else
                *h = 4 + ( r - g ) / delta; // between magenta & cyan
        *h *= 60; // degrees
        if( *h < 0 )
                *h += 360;
}

