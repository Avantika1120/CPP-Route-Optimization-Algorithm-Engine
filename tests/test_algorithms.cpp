#include "Algorithms.hpp"
#include "Graph.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    Graph g;
    g.addNode(1, 40.0, -74.0);
    g.addNode(2, 40.0, -73.999);
    g.addNode(3, 40.001, -73.999);
    g.addNode(4, 40.001, -74.0);

    g.addEdge(1, 2, 100.0);
    g.addEdge(2, 3, 100.0);
    g.addEdge(1, 4, 250.0);
    g.addEdge(4, 3, 100.0);
    g.addEdge(2, 1, 100.0);
    g.addEdge(3, 2, 100.0);
    g.addEdge(4, 1, 250.0);
    g.addEdge(3, 4, 100.0);

    auto d = dijkstra(g, 1, 3);
    assert(d.reachable);
    assert(std::abs(d.distance_m - 200.0) < 1e-9);
    assert(d.path.size() == 3);

    auto a = aStar(g, 1, 3);
    assert(a.reachable);
    assert(std::abs(a.distance_m - 200.0) < 1e-9);
    assert(a.path.front() == 1 && a.path.back() == 3);

    assert(g.nearestNode(40.0, -74.0) == 1);
    assert(g.nodeCount() == 4);
    assert(g.edgeCount() == 8);

    std::cout << "All route-engine tests passed.\n";
    return 0;
}
