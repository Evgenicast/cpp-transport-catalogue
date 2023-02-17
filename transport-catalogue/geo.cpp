#include "geo.h"
#include <cmath>

namespace geo
{
    double ComputeDistance(const Coordinates & From, const Coordinates & To)
    {
        using namespace std;
        const double dr = M_PI / 180.0;
        return acos(sin(From.lat * dr) * sin(To.lat * dr)
            + cos(From.lat * dr) * cos(To.lat * dr) * cos(abs(From.lng - To.lng) * dr))
            * EARTH_RADIUS;
    }
}  
