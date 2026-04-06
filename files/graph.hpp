#pragma once

#include <cstdint>
#include <vector>
#include <span>

namespace shortpath {

// Identificadores de OSM son enteros de 64 bits
using NodeId  = std::int64_t;
using EdgeIdx = std::uint32_t;

struct Edge {
    EdgeIdx  target;    // índice del nodo destino en el array de nodos
    float    weight;    // coste (tiempo en segundos, o distancia en metros)
};

// Grafo en formato CSR (Compressed Sparse Row) — caché-friendly.
// Una vez construido es inmutable; ideal para pathfinding de solo lectura.
struct Graph {
    // Coordenadas de cada nodo
    std::vector<double> lat;
    std::vector<double> lon;

    // CSR: head[i] es el índice en edges[] donde empiezan las aristas del nodo i
    // Las aristas del nodo i son edges[head[i] .. head[i+1])
    std::vector<EdgeIdx> head;
    std::vector<Edge>    edges;

    [[nodiscard]] std::size_t num_nodes() const noexcept { return lat.size(); }
    [[nodiscard]] std::size_t num_edges() const noexcept { return edges.size(); }

    [[nodiscard]] std::span<const Edge> neighbors(EdgeIdx node) const noexcept {
        return { edges.data() + head[node], edges.data() + head[node + 1] };
    }
};

} // namespace shortpath
