#pragma once

#include "../graph/graph.hpp"
#include <filesystem>
#include <string_view>

namespace shortpath {

enum class WeightMode {
    Distance,   // metros
    TravelTime, // segundos (usa maxspeed de OSM)
};

// Carga un fichero .osm.pbf y construye el grafo CSR.
// Lanza std::runtime_error si el fichero no existe o está corrupto.
[[nodiscard]] Graph load_osm(const std::filesystem::path& pbf_path,
                              WeightMode mode = WeightMode::TravelTime);

} // namespace shortpath
