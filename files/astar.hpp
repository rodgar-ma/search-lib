#pragma once

#include "../graph/graph.hpp"
#include <optional>
#include <functional>

namespace shortpath {

// A* con heurística haversine (admisible para grafos de carretera con pesos en metros).
[[nodiscard]] std::optional<PathResult>
astar(const Graph& graph, EdgeIdx source, EdgeIdx target);

} // namespace shortpath
