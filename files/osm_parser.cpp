#include "osm_parser.hpp"

#include <osmium/io/pbf_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/index/map/flex_mem.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/tags/tags_filter.hpp>

#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <algorithm>

namespace shortpath {

namespace {

// Velocidades por defecto (km/h) cuando maxspeed no está presente en OSM
float default_speed_kmh(std::string_view highway)
{
    if (highway == "motorway")       return 120.f;
    if (highway == "trunk")          return 100.f;
    if (highway == "primary")        return  80.f;
    if (highway == "secondary")      return  60.f;
    if (highway == "tertiary")       return  50.f;
    if (highway == "residential")    return  30.f;
    if (highway == "living_street")  return  10.f;
    return 50.f; // fallback
}

struct BuildState {
    // Mapa de OSM node_id -> índice interno
    std::unordered_map<osmium::object_id_type, uint32_t> id_to_idx;

    std::vector<double> lat;
    std::vector<double> lon;

    // Aristas temporales antes de construir CSR
    struct TempEdge {
        uint32_t from, to;
        float    weight;
    };
    std::vector<TempEdge> temp_edges;

    WeightMode mode;
};

class OsmHandler : public osmium::handler::Handler {
public:
    explicit OsmHandler(BuildState& state) : m_state(state) {}

    void way(const osmium::Way& way)
    {
        const char* highway = way.tags()["highway"];
        if (!highway) return;

        // Filtrar tipos no relevantes para routing en coche
        static const std::vector<std::string_view> routable = {
            "motorway", "trunk", "primary", "secondary", "tertiary",
            "unclassified", "residential", "living_street",
            "motorway_link", "trunk_link", "primary_link"
        };
        if (std::ranges::find(routable, highway) == routable.end()) return;

        const bool oneway = (way.tags().has_tag("oneway", "yes") ||
                             std::string_view(highway).ends_with("_link"));

        const float speed = [&]() -> float {
            const char* ms = way.tags()["maxspeed"];
            if (ms) {
                try { return std::stof(ms); } catch (...) {}
            }
            return default_speed_kmh(highway);
        }();

        const auto& nodes = way.nodes();
        for (std::size_t i = 0; i + 1 < nodes.size(); ++i) {
            if (!nodes[i].location().valid() || !nodes[i+1].location().valid())
                continue;

            auto& s = m_state;

            auto register_node = [&](const osmium::NodeRef& nr) -> uint32_t {
                auto [it, inserted] = s.id_to_idx.emplace(nr.ref(),
                    static_cast<uint32_t>(s.lat.size()));
                if (inserted) {
                    s.lat.push_back(nr.location().lat());
                    s.lon.push_back(nr.location().lon());
                }
                return it->second;
            };

            const uint32_t a = register_node(nodes[i]);
            const uint32_t b = register_node(nodes[i+1]);

            // Distancia haversine aproximada entre nodos consecutivos
            const float dist = [&]() -> float {
                const double dlat = (nodes[i+1].location().lat() - nodes[i].location().lat()) * (3.14159265358979 / 180.0);
                const double dlon = (nodes[i+1].location().lon() - nodes[i].location().lon()) * (3.14159265358979 / 180.0);
                const double rlat = nodes[i].location().lat() * (3.14159265358979 / 180.0);
                const double sa = std::sin(dlat/2)*std::sin(dlat/2)
                                + std::cos(rlat)*std::cos(rlat + dlat)
                                * std::sin(dlon/2)*std::sin(dlon/2);
                return static_cast<float>(6'371'000.0 * 2.0 * std::atan2(std::sqrt(sa), std::sqrt(1.0 - sa)));
            }();

            const float weight = (s.mode == WeightMode::Distance)
                ? dist
                : dist / (speed / 3.6f); // metros / (m/s) = segundos

            s.temp_edges.push_back({ a, b, weight });
            if (!oneway)
                s.temp_edges.push_back({ b, a, weight });
        }
    }

private:
    BuildState& m_state;
};

} // anonymous namespace

Graph load_osm(const std::filesystem::path& pbf_path, WeightMode mode)
{
    if (!std::filesystem::exists(pbf_path))
        throw std::runtime_error("Fichero no encontrado: " + pbf_path.string());

    BuildState state;
    state.mode = mode;

    using Index = osmium::index::map::FlexMem<osmium::unsigned_object_id_type,
                                               osmium::Location>;
    using LocationHandler = osmium::handler::NodeLocationsForWays<Index>;

    osmium::io::Reader reader{ pbf_path, osmium::osm_entity_bits::node |
                                          osmium::osm_entity_bits::way };
    Index index;
    LocationHandler location_handler{ index };
    OsmHandler osm_handler{ state };
    osmium::apply(reader, location_handler, osm_handler);
    reader.close();

    // Construir grafo CSR
    const uint32_t n = static_cast<uint32_t>(state.lat.size());
    Graph g;
    g.lat  = std::move(state.lat);
    g.lon  = std::move(state.lon);
    g.head.assign(n + 1, 0);

    // Contar grado de salida de cada nodo
    for (const auto& e : state.temp_edges)
        ++g.head[e.from + 1];

    // Prefixsum
    for (uint32_t i = 1; i <= n; ++i)
        g.head[i] += g.head[i-1];

    // Insertar aristas
    g.edges.resize(state.temp_edges.size());
    std::vector<uint32_t> pos(g.head.begin(), g.head.end());
    for (const auto& e : state.temp_edges) {
        g.edges[pos[e.from]++] = { e.to, e.weight };
    }

    return g;
}

} // namespace shortpath
