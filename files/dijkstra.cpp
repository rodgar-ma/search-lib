#include "dijkstra.hpp"
#include <queue>
#include <limits>

namespace shortpath {

namespace {
    struct PQEntry {
        float    cost;
        EdgeIdx  node;
        bool operator>(const PQEntry& o) const noexcept { return cost > o.cost; }
    };
}

std::optional<PathResult>
dijkstra(const Graph& graph, EdgeIdx source, EdgeIdx target)
{
    const auto n = static_cast<EdgeIdx>(graph.num_nodes());

    std::vector<float>   dist(n, std::numeric_limits<float>::infinity());
    std::vector<EdgeIdx> prev(n, std::numeric_limits<EdgeIdx>::max());

    std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<>> pq;
    dist[source] = 0.0f;
    pq.push({ 0.0f, source });

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();

        if (u == target) break;
        if (d > dist[u]) continue; // entrada obsoleta

        for (const auto& e : graph.neighbors(u)) {
            const float nd = dist[u] + e.weight;
            if (nd < dist[e.target]) {
                dist[e.target] = nd;
                prev[e.target] = u;
                pq.push({ nd, e.target });
            }
        }
    }

    if (dist[target] == std::numeric_limits<float>::infinity())
        return std::nullopt;

    // Reconstruir camino
    PathResult result{ .cost = dist[target] };
    for (EdgeIdx v = target; v != std::numeric_limits<EdgeIdx>::max(); v = prev[v])
        result.nodes.push_back(v);
    std::ranges::reverse(result.nodes);

    return result;
}

} // namespace shortpath
