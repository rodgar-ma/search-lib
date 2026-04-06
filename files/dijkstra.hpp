#pragma once

#include "../graph/graph.hpp"
#include <cstdint>
#include <vector>
#include <optional>

namespace shortpath {

struct PathResult {
    float                cost;       // coste total del camino
    std::vector<EdgeIdx> nodes;      // secuencia de índices de nodos
};

// Dijkstra estándar desde 'source' hacia 'target'.
// Devuelve nullopt si no existe camino.
[[nodiscard]] std::optional<PathResult>
dijkstra(const Graph& graph, EdgeIdx source, EdgeIdx target);

} // namespace shortpath
