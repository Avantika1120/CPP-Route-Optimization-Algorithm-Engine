#include "Algorithms.hpp"

#include <algorithm>
#include <functional>
#include <queue>
#include <unordered_map>

namespace {
using Item = std::pair<double, std::int64_t>;

std::vector<std::int64_t> reconstruct(std::int64_t source,
                                      std::int64_t target,
                                      const std::unordered_map<std::int64_t, std::int64_t>& parent) {
    std::vector<std::int64_t> path;
    std::int64_t current = target;
    path.push_back(current);
    while (current != source) {
        auto it = parent.find(current);
        if (it == parent.end()) return {};
        current = it->second;
        path.push_back(current);
    }
    std::reverse(path.begin(), path.end());
    return path;
}
}

PathResult dijkstra(const Graph& graph, std::int64_t source, std::int64_t target) {
    if (!graph.hasNode(source) || !graph.hasNode(target)) return {};

    std::unordered_map<std::int64_t, double> dist;
    std::unordered_map<std::int64_t, std::int64_t> parent;
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
    dist[source] = 0.0;
    pq.push({0.0, source});
    std::size_t expanded = 0;

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d != dist[u]) continue;
        ++expanded;
        if (u == target) break;

        for (const auto& edge : graph.neighbors(u)) {
            const double nd = d + edge.distance_m;
            if (!dist.count(edge.to) || nd < dist[edge.to]) {
                dist[edge.to] = nd;
                parent[edge.to] = u;
                pq.push({nd, edge.to});
            }
        }
    }

    if (!dist.count(target)) return {0.0, {}, expanded, false};
    return {dist[target], reconstruct(source, target, parent), expanded, true};
}

PathResult aStar(const Graph& graph, std::int64_t source, std::int64_t target) {
    if (!graph.hasNode(source) || !graph.hasNode(target)) return {};

    std::unordered_map<std::int64_t, double> g_score;
    std::unordered_map<std::int64_t, std::int64_t> parent;
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open;
    const auto& goal = graph.node(target);
    g_score[source] = 0.0;
    const auto& start = graph.node(source);
    open.push({haversineMeters(start.lat, start.lon, goal.lat, goal.lon), source});
    std::size_t expanded = 0;

    while (!open.empty()) {
        const auto [f, u] = open.top();
        open.pop();

        const auto& u_node = graph.node(u);
        const double expected_f = g_score[u] +
            haversineMeters(u_node.lat, u_node.lon, goal.lat, goal.lon);
        if (f > expected_f + 1e-9) continue;

        ++expanded;
        if (u == target) break;

        for (const auto& edge : graph.neighbors(u)) {
            const double tentative = g_score[u] + edge.distance_m;
            if (!g_score.count(edge.to) || tentative < g_score[edge.to]) {
                g_score[edge.to] = tentative;
                parent[edge.to] = u;
                const auto& n = graph.node(edge.to);
                const double h = haversineMeters(n.lat, n.lon, goal.lat, goal.lon);
                open.push({tentative + h, edge.to});
            }
        }
    }

    if (!g_score.count(target)) return {0.0, {}, expanded, false};
    return {g_score[target], reconstruct(source, target, parent), expanded, true};
}
