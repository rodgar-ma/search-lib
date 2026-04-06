#pragma once

namespace shortpath::geo {

// Distancia haversine entre dos coordenadas en metros.
// Usada como heurística admisible para A*.
[[nodiscard]] double haversine(double lat1, double lon1,
                                double lat2, double lon2) noexcept;

} // namespace shortpath::geo
