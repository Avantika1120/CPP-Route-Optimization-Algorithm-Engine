#pragma once

#include "Graph.hpp"

#include <cstdint>
#include <vector>

struct PathResult {
    double distance_m{0.0};
    std::vector<std::int64_t> path;
    std::size_t expanded_nodes{0};
    bool reachable{false};
};

PathResult dijkstra(const Graph& graph, std::int64_t source, std::int64_t target);
PathResult aStar(const Graph& graph, std::int64_t source, std::int64_t target);
