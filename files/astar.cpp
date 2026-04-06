#include "astar.hpp"
#include "dijkstra.hpp"   // reutiliza PathResult
#include "../utils/geo.hpp"
#include <queue>
#include <limits>

namespace shortpath {

namespace {
    struct PQEntry {
        float    f;    // g + h
        float    g;    // coste real desde source
        EdgeIdx  node;
        bool operator>(const PQEntry& o) const noexcept { return f > o.f; }
    };
}

std::optional<PathResult>
astar(const Graph& graph, EdgeIdx source, EdgeIdx target)
{
    const auto n = static_cast<EdgeIdx>(graph.num_nodes());

    std::vector<float>   g(n, std::numeric_limits<float>::infinity());
    std::vector<EdgeIdx> prev(n, std::numeric_limits<EdgeIdx>::max());

    const double tlat = graph.lat[target];
    const double tlon = graph.lon[target];

    auto heuristic = [&](EdgeIdx node) -> float {
        return static_cast<float>(geo::haversine(graph.lat[node], graph.lon[node], tlat, tlon));
    };

    std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<>> pq;
    g[source] = 0.0f;
    pq.push({ heuristic(source), 0.0f, source });

    while (!pq.empty()) {
        auto [f, gv, u] = pq.top(); pq.pop();

        if (u == target) break;
        if (gv > g[u]) continue;

        for (const auto& e : graph.neighbors(u)) {
            const float ng = g[u] + e.weight;
            if (ng < g[e.target]) {
                g[e.target] = ng;
                prev[e.target] = u;
                pq.push({ ng + heuristic(e.target), ng, e.target });
            }
        }
    }

    if (g[target] == std::numeric_limits<float>::infinity())
        return std::nullopt;

    PathResult result{ .cost = g[target] };
    for (EdgeIdx v = target; v != std::numeric_limits<EdgeIdx>::max(); v = prev[v])
        result.nodes.push_back(v);
    std::ranges::reverse(result.nodes);

    return result;
}

} // namespace shortpath
