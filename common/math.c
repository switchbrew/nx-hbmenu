#include "math.h"

// Low precision, adapted from http://lab.polygonal.de/2007/07/18/fast-and-accurate-sinecosine-approximation/
float approxSin(float x) {
    float ret;

    // always wrap input angle to -PI..PI
    while (x<-3.14159265 || x>3.14159265) {
        if (x<-3.14159265)
            x += 6.28318531;
        else if (x >3.14159265)
            x -= 6.28318531;
    }

    // compute sine
    if (x<0)
        ret = 1.27323954 * x + .405284735 * x * x;
    else
        ret = 1.27323954 * x - 0.405284735 * x * x;

    return ret;
}