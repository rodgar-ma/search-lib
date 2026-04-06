#include <benchmark/benchmark.h>
#include "../src/graph/graph.hpp"
#include "../src/algorithms/dijkstra.hpp"
#include "../src/algorithms/astar.hpp"

using namespace shortpath;

// Genera un grafo de cuadrícula n x n para benchmarks reproducibles
static Graph make_grid(int n)
{
    Graph g;
    const int total = n * n;
    g.lat.resize(total); g.lon.resize(total);
    g.head.resize(total + 1, 0);

    std::vector<std::pair<uint32_t,float>> tmp_edges;
    tmp_edges.reserve(total * 4);

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            const int idx = r * n + c;
            g.lat[idx] = r * 0.01;
            g.lon[idx] = c * 0.01;
            if (c + 1 < n) { tmp_edges.push_back({static_cast<uint32_t>(idx * 4 + 0), 0.0f}); }
            if (r + 1 < n) { tmp_edges.push_back({static_cast<uint32_t>(idx * 4 + 1), 0.0f}); }
        }
    }

    // Build proper CSR
    std::vector<std::tuple<uint32_t,uint32_t,float>> edges;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            uint32_t u = r * n + c;
            if (c + 1 < n) edges.push_back({u, u + 1, 1.0f});
            if (r + 1 < n) edges.push_back({u, static_cast<uint32_t>(u + n), 1.0f});
        }
    }

    g.head.assign(total + 1, 0);
    for (auto& [f, t, w] : edges) ++g.head[f + 1];
    for (int i = 1; i <= total; ++i) g.head[i] += g.head[i-1];

    g.edges.resize(edges.size());
    std::vector<uint32_t> pos(g.head.begin(), g.head.end());
    for (auto& [f, t, w] : edges) g.edges[pos[f]++] = {t, w};

    return g;
}

static void BM_Dijkstra_Grid100(benchmark::State& state)
{
    const auto g = make_grid(100);
    for (auto _ : state)
        benchmark::DoNotOptimize(dijkstra(g, 0, 99 * 100 + 99));
}
BENCHMARK(BM_Dijkstra_Grid100);

static void BM_AStar_Grid100(benchmark::State& state)
{
    const auto g = make_grid(100);
    for (auto _ : state)
        benchmark::DoNotOptimize(astar(g, 0, 99 * 100 + 99));
}
BENCHMARK(BM_AStar_Grid100);
