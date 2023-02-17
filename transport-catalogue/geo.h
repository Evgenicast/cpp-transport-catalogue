#pragma once

const int EARTH_RADIUS = 6371000;

namespace geo
{
    struct Coordinates
    {
        double lat;
        double lng;
    };

    double ComputeDistance(const Coordinates & From, const Coordinates & To);
}  // namespace geo
