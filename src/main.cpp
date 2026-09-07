#include "Algorithms.hpp"
#include "Graph.hpp"
#include "RouteOptimizer.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    try {
        const std::string nodes = argc > 1 ? argv[1] : "data/nodes.csv";
        const std::string edges = argc > 2 ? argv[2] : "data/edges.csv";
        const std::string deliveries = argc > 3 ? argv[3] : "data/deliveries.csv";

        Graph graph = Graph::fromCsv(nodes, edges);
        auto stops = loadDeliveries(deliveries, graph);
        if (stops.empty()) {
            std::cerr << "No delivery stops found.\n";
            return 1;
        }

        // Depot is a real Manhattan location near Times Square.
        const std::int64_t depot = graph.nearestNode(40.7580, -73.9855);

        std::cout << "Loaded road graph: " << graph.nodeCount() << " nodes, "
                  << graph.edgeCount() << " directed edges\n";
        std::cout << "Delivery stops: " << stops.size() << "\n\n";

        if (stops.size() >= 2) {
            const auto source = stops.front().node_id;
            const auto target = stops.back().node_id;

            auto t1 = std::chrono::high_resolution_clock::now();
            auto dijkstra_result = dijkstra(graph, source, target);
            auto t2 = std::chrono::high_resolution_clock::now();
            auto astar_result = aStar(graph, source, target);
            auto t3 = std::chrono::high_resolution_clock::now();

            const auto dijkstra_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            const auto astar_us = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();

            std::cout << "Shortest-path benchmark\n";
            std::cout << "Dijkstra: distance=" << std::fixed << std::setprecision(2)
                      << dijkstra_result.distance_m / 1000.0 << " km, expanded="
                      << dijkstra_result.expanded_nodes << ", time=" << dijkstra_us << " us\n";
            std::cout << "A*:       distance=" << astar_result.distance_m / 1000.0
                      << " km, expanded=" << astar_result.expanded_nodes
                      << ", time=" << astar_us << " us\n\n";
        }

        const double baseline = routeDistance(graph, depot, stops);
        const auto optimized = optimizeNearestNeighbor2Opt(graph, depot, stops);

        std::cout << "Multi-stop delivery optimization\n";
        std::cout << "Baseline input-order distance: " << baseline / 1000.0 << " km\n";
        std::cout << "Optimized distance:            " << optimized.total_distance_m / 1000.0 << " km\n";
        if (baseline > 0.0 && baseline < 1e100) {
            std::cout << "Distance reduction:            "
                      << (baseline - optimized.total_distance_m) / baseline * 100.0 << "%\n";
        }

        std::cout << "\nOptimized stop sequence:\n";
        for (std::size_t i = 0; i < optimized.ordered_stops.size(); ++i) {
            const auto& stop = optimized.ordered_stops[i];
            std::cout << i + 1 << ". " << stop.name << " (priority " << stop.priority << ")\n";
        }
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
