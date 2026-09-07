#include "Graph.hpp"

#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace {
constexpr double kEarthRadiusM = 6371000.0;
constexpr double kPi = 3.14159265358979323846;

double deg2rad(double degrees) {
    return degrees * kPi / 180.0;
}
}

double haversineMeters(double lat1, double lon1, double lat2, double lon2) {
    const double dlat = deg2rad(lat2 - lat1);
    const double dlon = deg2rad(lon2 - lon1);
    const double a = std::sin(dlat / 2.0) * std::sin(dlat / 2.0) +
                     std::cos(deg2rad(lat1)) * std::cos(deg2rad(lat2)) *
                     std::sin(dlon / 2.0) * std::sin(dlon / 2.0);
    const double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    return kEarthRadiusM * c;
}

void Graph::addNode(std::int64_t id, double lat, double lon) {
    nodes_[id] = Node{id, lat, lon};
    adjacency_.try_emplace(id);
}

void Graph::addEdge(std::int64_t from, std::int64_t to, double distance_m) {
    if (!hasNode(from) || !hasNode(to)) {
        throw std::runtime_error("Edge references a node that does not exist");
    }
    adjacency_[from].push_back(Edge{to, distance_m});
    ++edge_count_;
}

bool Graph::hasNode(std::int64_t id) const {
    return nodes_.find(id) != nodes_.end();
}

const Node& Graph::node(std::int64_t id) const {
    return nodes_.at(id);
}

const std::vector<Edge>& Graph::neighbors(std::int64_t id) const {
    static const std::vector<Edge> empty;
    auto it = adjacency_.find(id);
    return it == adjacency_.end() ? empty : it->second;
}

std::size_t Graph::nodeCount() const {
    return nodes_.size();
}

std::size_t Graph::edgeCount() const {
    return edge_count_;
}

Graph Graph::fromCsv(const std::string& nodes_path, const std::string& edges_path) {
    Graph graph;
    std::ifstream nodes_file(nodes_path);
    if (!nodes_file) {
        throw std::runtime_error("Unable to open nodes CSV: " + nodes_path);
    }

    std::string line;
    std::getline(nodes_file, line);
    while (std::getline(nodes_file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string id, lat, lon;
        std::getline(ss, id, ',');
        std::getline(ss, lat, ',');
        std::getline(ss, lon, ',');
        graph.addNode(std::stoll(id), std::stod(lat), std::stod(lon));
    }

    std::ifstream edges_file(edges_path);
    if (!edges_file) {
        throw std::runtime_error("Unable to open edges CSV: " + edges_path);
    }

    std::getline(edges_file, line);
    while (std::getline(edges_file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string from, to, distance;
        std::getline(ss, from, ',');
        std::getline(ss, to, ',');
        std::getline(ss, distance, ',');
        graph.addEdge(std::stoll(from), std::stoll(to), std::stod(distance));
    }

    return graph;
}

std::int64_t Graph::nearestNode(double lat, double lon) const {
    if (nodes_.empty()) {
        throw std::runtime_error("Cannot search nearest node in an empty graph");
    }

    double best_distance = std::numeric_limits<double>::infinity();
    std::int64_t best_id = nodes_.begin()->first;
    for (const auto& [id, n] : nodes_) {
        const double d = haversineMeters(lat, lon, n.lat, n.lon);
        if (d < best_distance) {
            best_distance = d;
            best_id = id;
        }
    }
    return best_id;
}
