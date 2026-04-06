#include "geo.hpp"
#include <cmath>

namespace shortpath::geo {

static constexpr double EARTH_RADIUS_M = 6'371'000.0;
static constexpr double DEG2RAD        = std::numbers::pi / 180.0;

double haversine(double lat1, double lon1,
                 double lat2, double lon2) noexcept
{
    const double dlat = (lat2 - lat1) * DEG2RAD;
    const double dlon = (lon2 - lon1) * DEG2RAD;
    const double rlat1 = lat1 * DEG2RAD;
    const double rlat2 = lat2 * DEG2RAD;

    const double a = std::sin(dlat / 2) * std::sin(dlat / 2)
                   + std::cos(rlat1) * std::cos(rlat2)
                   * std::sin(dlon / 2) * std::sin(dlon / 2);

    return EARTH_RADIUS_M * 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
}

} // namespace shortpath::geo
