#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct Node {
    std::int64_t id{};
    double lat{};
    double lon{};
};

struct Edge {
    std::int64_t to{};
    double distance_m{};
};

class Graph {
public:
    void addNode(std::int64_t id, double lat, double lon);
    void addEdge(std::int64_t from, std::int64_t to, double distance_m);

    bool hasNode(std::int64_t id) const;
    const Node& node(std::int64_t id) const;
    const std::vector<Edge>& neighbors(std::int64_t id) const;

    std::size_t nodeCount() const;
    std::size_t edgeCount() const;

    static Graph fromCsv(const std::string& nodes_path, const std::string& edges_path);
    std::int64_t nearestNode(double lat, double lon) const;

private:
    std::unordered_map<std::int64_t, Node> nodes_;
    std::unordered_map<std::int64_t, std::vector<Edge>> adjacency_;
    std::size_t edge_count_{};
};

double haversineMeters(double lat1, double lon1, double lat2, double lon2);
