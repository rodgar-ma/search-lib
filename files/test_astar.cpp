#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/graph/graph.hpp"
#include "../src/algorithms/dijkstra.hpp"
#include "../src/algorithms/astar.hpp"

using namespace shortpath;
using Catch::Matchers::WithinRel;

// Grafo lineal: 4 nodos en línea recta con coordenadas reales aproximadas
static Graph make_linear()
{
    // Nodos aproximadamente cada ~1km en dirección este (Vigo, zona ejemplo)
    Graph g;
    g.lat = { 42.2328, 42.2328, 42.2328, 42.2328 };
    g.lon = { -8.7207, -8.7117, -8.7027, -8.6937 }; // ~100m entre nodos
    // Pesos en metros (haversine aproximado)
    g.head = { 0, 1, 2, 3, 3 };
    g.edges = { { 1, 700.f }, { 2, 700.f }, { 3, 700.f } };
    return g;
}

TEST_CASE("A* produce el mismo coste que Dijkstra", "[astar]")
{
    const auto g = make_linear();
    const auto d = dijkstra(g, 0, 3);
    const auto a = astar(g, 0, 3);

    REQUIRE(d.has_value());
    REQUIRE(a.has_value());
    REQUIRE_THAT(a->cost, WithinRel(d->cost, 1e-4f));
}

TEST_CASE("A*: sin camino devuelve nullopt", "[astar]")
{
    const auto g = make_linear();
    const auto result = astar(g, 3, 0);
    REQUIRE_FALSE(result.has_value());
}
