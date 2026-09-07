# Project Case Study — C++ Route Optimization Engine

## 1. Business problem

A delivery operation has a depot and multiple customer stops across Manhattan. Visiting the stops in arbitrary input order wastes distance. The technical problem is to combine fast point-to-point routing with a practical multi-stop scheduling heuristic.

## 2. Data acquisition

Rather than using an artificial classroom graph, the project downloads Manhattan road data from OpenStreetMap through the Overpass API.

The ingestion script:

1. requests drivable `highway` ways inside a Manhattan bounding box;
2. collects all referenced OSM nodes and coordinates;
3. preserves one-way road direction where indicated;
4. computes each road-segment length using the Haversine formula;
5. exports node and directed-edge CSV files.

The C++ application therefore receives a sparse weighted street graph built from public real-world data.

## 3. Graph design in C++

The road network is stored as an adjacency list:

- key: OSM node ID
- value: outgoing `Edge` objects
- edge weight: road segment distance in meters

An adjacency list is appropriate because urban road networks are sparse compared with all possible node-to-node connections.

## 4. Shortest-path baseline — Dijkstra

Dijkstra was implemented from first principles with `std::priority_queue`, hash maps for distances/parents, and path reconstruction.

It serves as the correctness baseline because all edge weights are non-negative.

## 5. Guided search — A*

A* uses straight-line Haversine distance from the current road node to the target as its heuristic. This adds geographic direction to the search while keeping the same road-distance objective.

The program records elapsed microseconds and number of expanded nodes so Dijkstra and A* can be compared empirically on the actual downloaded road graph.

## 6. Multi-stop route optimization

Point-to-point shortest path does not decide the best order for many stops. The optimizer therefore uses two stages.

### Stage A — priority-aware nearest neighbor

Starting from the depot, the engine repeatedly selects a nearby unserved stop. A modest priority factor lets urgent orders receive preference without completely ignoring route distance.

### Stage B — 2-opt

The first route is improved by reversing pairs of route segments. A candidate is accepted only if total road distance decreases.

This does not claim an exact globally optimal TSP solution. The design intentionally demonstrates a fast, explainable heuristic plus local search.

## 7. Evaluation

The executable reports two groups of metrics.

### Search metrics
- Dijkstra road distance
- A* road distance
- expanded nodes
- execution time

### Route metrics
- baseline input-order delivery distance
- optimized delivery distance
- percentage distance reduction
- final stop sequence

Metrics are calculated at runtime rather than written as fixed portfolio claims.

## 8. Engineering practices

The repository also includes:

- CMake build configuration
- modular headers/source files
- deterministic unit tests on a small graph
- compiler warnings (`-Wall -Wextra -Wpedantic`)
- GitHub Actions build/test CI
- real-data ingestion documentation
- algorithm complexity analysis

## 9. What I would improve in a production system

The current version is designed to make core algorithms visible and understandable. A production engine could add pairwise route caching, traffic-dependent weights, time windows, vehicle capacity constraints, contraction hierarchies, persistent spatial indexing, and a map visualization layer.
