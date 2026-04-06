# shortpath

Librería C++23 de algoritmos de búsqueda del camino más corto para mapas de carretera usando OpenStreetMap.

## Algoritmos implementados

- Dijkstra
- A* (heurística haversine)
- _Próximamente: Bidirectional Dijkstra, Contraction Hierarchies_

## Dependencias

| Librería           | Tipo          | Uso                          |
|--------------------|---------------|------------------------------|
| libosmium          | Submódulo     | Parsing de ficheros .osm.pbf |
| protozero          | Submódulo     | Dependencia de libosmium     |
| Catch2             | Submódulo     | Tests                        |
| google/benchmark   | Submódulo     | Benchmarks                   |
| zlib, bzip2        | Sistema       | Compresión PBF               |

## Inicio rápido

```bash
# Clonar con submódulos
git clone --recurse-submodules https://github.com/tu-usuario/shortpath.git
cd shortpath

# Añadir submódulos si ya tenías el repo
git submodule update --init --recursive

# Build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Tests
ctest --test-dir build

# Benchmarks
./build/benchmarks/shortpath_benchmarks
```

## Datos de prueba

Descarga un extracto de Galicia desde Geofabrik:

```bash
wget -P data/ https://download.geofabrik.de/europe/spain/galicia-latest.osm.pbf
```

## Uso básico

```cpp
#include <shortpath/shortpath.hpp>

int main()
{
    auto graph = shortpath::load_osm("data/galicia-latest.osm.pbf",
                                     shortpath::WeightMode::TravelTime);

    auto result = shortpath::dijkstra(graph, 0, 1000);
    if (result)
        std::println("Coste: {} segundos, {} nodos", result->cost, result->nodes.size());
}
```
