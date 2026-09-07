#include "RouteOptimizer.hpp"

#include <algorithm>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>

std::vector<DeliveryStop> loadDeliveries(const std::string& csv_path, const Graph& graph) {
    std::ifstream file(csv_path);
    if (!file) throw std::runtime_error("Unable to open deliveries CSV: " + csv_path);

    std::vector<DeliveryStop> stops;
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string name, lat, lon, priority;
        std::getline(ss, name, ',');
        std::getline(ss, lat, ',');
        std::getline(ss, lon, ',');
        std::getline(ss, priority, ',');
        DeliveryStop stop{name, std::stod(lat), std::stod(lon), std::stoi(priority), 0};
        stop.node_id = graph.nearestNode(stop.lat, stop.lon);
        stops.push_back(stop);
    }
    return stops;
}

double routeDistance(const Graph& graph,
                     std::int64_t depot_node,
                     const std::vector<DeliveryStop>& stops) {
    if (stops.empty()) return 0.0;
    double total = 0.0;
    std::int64_t current = depot_node;
    for (const auto& stop : stops) {
        auto result = aStar(graph, current, stop.node_id);
        if (!result.reachable) return std::numeric_limits<double>::infinity();
        total += result.distance_m;
        current = stop.node_id;
    }
    auto back = aStar(graph, current, depot_node);
    if (!back.reachable) return std::numeric_limits<double>::infinity();
    return total + back.distance_m;
}

OptimizedRoute optimizeNearestNeighbor2Opt(const Graph& graph,
                                           std::int64_t depot_node,
                                           const std::vector<DeliveryStop>& stops) {
    std::vector<DeliveryStop> remaining = stops;
    std::vector<DeliveryStop> route;
    std::int64_t current = depot_node;

    while (!remaining.empty()) {
        std::size_t best_index = 0;
        double best_score = std::numeric_limits<double>::infinity();

        for (std::size_t i = 0; i < remaining.size(); ++i) {
            auto result = aStar(graph, current, remaining[i].node_id);
            if (!result.reachable) continue;
            // Higher priority deliveries receive a modest score advantage.
            const double priority_factor = 1.0 + 0.15 * std::max(0, remaining[i].priority - 1);
            const double score = result.distance_m / priority_factor;
            if (score < best_score) {
                best_score = score;
                best_index = i;
            }
        }

        route.push_back(remaining[best_index]);
        current = remaining[best_index].node_id;
        remaining.erase(remaining.begin() + static_cast<long>(best_index));
    }

    double best_distance = routeDistance(graph, depot_node, route);
    bool improved = true;
    while (improved) {
        improved = false;
        for (std::size_t i = 0; i + 1 < route.size(); ++i) {
            for (std::size_t k = i + 1; k < route.size(); ++k) {
                auto candidate = route;
                std::reverse(candidate.begin() + static_cast<long>(i),
                             candidate.begin() + static_cast<long>(k) + 1);
                const double candidate_distance = routeDistance(graph, depot_node, candidate);
                if (candidate_distance + 1e-6 < best_distance) {
                    route = std::move(candidate);
                    best_distance = candidate_distance;
                    improved = true;
                }
            }
        }
    }

    return {route, best_distance};
}
