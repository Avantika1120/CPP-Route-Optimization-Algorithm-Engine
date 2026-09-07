#pragma once

#include "Algorithms.hpp"
#include "Graph.hpp"

#include <cstdint>
#include <string>
#include <vector>

struct DeliveryStop {
    std::string name;
    double lat{};
    double lon{};
    int priority{1};
    std::int64_t node_id{};
};

struct OptimizedRoute {
    std::vector<DeliveryStop> ordered_stops;
    double total_distance_m{0.0};
};

std::vector<DeliveryStop> loadDeliveries(const std::string& csv_path, const Graph& graph);
OptimizedRoute optimizeNearestNeighbor2Opt(const Graph& graph,
                                           std::int64_t depot_node,
                                           const std::vector<DeliveryStop>& stops);
double routeDistance(const Graph& graph,
                     std::int64_t depot_node,
                     const std::vector<DeliveryStop>& stops);
