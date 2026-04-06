#include <catch2/catch_test_macros.hpp>
#include "../src/graph/graph.hpp"

using namespace shortpath;

// Construcción manual de un grafo pequeño para tests
static Graph make_triangle()
{
    // 3 nodos, 3 aristas (A->B, B->C, A->C)
    //   A(0) --1.0-- B(1)
    //    \           |
    //    2.0        1.0
    //      \         |
    //       -------> C(2)
    Graph g;
    g.lat = { 0.0, 1.0, 1.0 };
    g.lon = { 0.0, 0.0, 1.0 };
    g.head = { 0, 2, 3, 3 }; // nodo 0: aristas [0,2), nodo 1: [2,3), nodo 2: [3,3)
    g.edges = {
        { 1, 1.0f }, // A->B
        { 2, 2.0f }, // A->C
        { 2, 1.0f }, // B->C
    };
    return g;
}

TEST_CASE("Graph: num_nodes y num_edges", "[graph]")
{
    const auto g = make_triangle();
    REQUIRE(g.num_nodes() == 3);
    REQUIRE(g.num_edges() == 3);
}

TEST_CASE("Graph: neighbors devuelve aristas correctas", "[graph]")
{
    const auto g = make_triangle();

    auto nb0 = g.neighbors(0);
    REQUIRE(nb0.size() == 2);
    REQUIRE(nb0[0].target == 1);
    REQUIRE(nb0[0].weight == 1.0f);
    REQUIRE(nb0[1].target == 2);
    REQUIRE(nb0[1].weight == 2.0f);

    auto nb1 = g.neighbors(1);
    REQUIRE(nb1.size() == 1);
    REQUIRE(nb1[0].target == 2);

    auto nb2 = g.neighbors(2);
    REQUIRE(nb2.empty());
}
