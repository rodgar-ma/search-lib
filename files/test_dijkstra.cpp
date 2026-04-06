#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/graph/graph.hpp"
#include "../src/algorithms/dijkstra.hpp"

using namespace shortpath;
using Catch::Matchers::WithinAbs;

static Graph make_triangle()
{
    Graph g;
    g.lat = { 0.0, 1.0, 1.0 };
    g.lon = { 0.0, 0.0, 1.0 };
    g.head = { 0, 2, 3, 3 };
    g.edges = { { 1, 1.0f }, { 2, 2.0f }, { 2, 1.0f } };
    return g;
}

TEST_CASE("Dijkstra: camino directo", "[dijkstra]")
{
    const auto g = make_triangle();
    const auto result = dijkstra(g, 0, 1);
    REQUIRE(result.has_value());
    REQUIRE_THAT(result->cost, WithinAbs(1.0f, 1e-5f));
    REQUIRE(result->nodes == std::vector<EdgeIdx>{ 0, 1 });
}

TEST_CASE("Dijkstra: camino indirecto más corto", "[dijkstra]")
{
    // A->C directo cuesta 2.0, pero A->B->C cuesta 1.0+1.0=2.0 igual.
    // Ambas rutas son válidas. Solo verificamos el coste.
    const auto g = make_triangle();
    const auto result = dijkstra(g, 0, 2);
    REQUIRE(result.has_value());
    REQUIRE_THAT(result->cost, WithinAbs(2.0f, 1e-5f));
}

TEST_CASE("Dijkstra: sin camino devuelve nullopt", "[dijkstra]")
{
    const auto g = make_triangle();
    const auto result = dijkstra(g, 2, 0); // grafo dirigido, no hay vuelta
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("Dijkstra: source == target", "[dijkstra]")
{
    const auto g = make_triangle();
    const auto result = dijkstra(g, 1, 1);
    REQUIRE(result.has_value());
    REQUIRE_THAT(result->cost, WithinAbs(0.0f, 1e-5f));
    REQUIRE(result->nodes == std::vector<EdgeIdx>{ 1 });
}
