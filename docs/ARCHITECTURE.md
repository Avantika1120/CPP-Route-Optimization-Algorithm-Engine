# Architecture

## Goal

Build a reproducible C++ route-optimization engine that runs on a **real Manhattan road network** instead of a toy graph.

## Data flow

```text
OpenStreetMap / Overpass API
        |
        v
scripts/download_osm.py
        |
        +--> data/nodes.csv
        |      node_id, latitude, longitude
        |
        +--> data/edges.csv
               from, to, distance_m
        |
        v
C++ Graph loader
        |
        +--> adjacency-list graph
        |
        +--> nearest-node geospatial lookup
        |
        v
Shortest-path layer
        |
        +--> Dijkstra
        +--> A* with Haversine heuristic
        |
        v
Multi-stop optimization
        |
        +--> priority-aware nearest-neighbor seed
        +--> 2-opt local search
        |
        v
CLI output / benchmarks
```

## Core components

### `Graph`
Stores OSM road nodes and directed road segments using an adjacency list. Edge weights are physical segment distances in meters. This keeps shortest-path operations sparse and memory-efficient for a street network.

### `Dijkstra`
Provides the correctness baseline for non-negative road weights. It uses a min-priority queue and maintains tentative distances and parent pointers.

### `A*`
Uses the same weighted road graph but adds straight-line Haversine distance to the destination as an admissible geographic heuristic. On a large city graph this can reduce unnecessary node expansion while preserving the optimal shortest path.

### Route optimizer
A route with several delivery stops is a different problem from a single shortest path. The engine therefore:

1. snaps each delivery location to its closest road node;
2. creates a route with a priority-aware nearest-neighbor heuristic;
3. measures route cost using A* shortest-path distance between consecutive stops;
4. repeatedly applies 2-opt segment reversals when they reduce total road distance;
5. returns to the depot and reports total route distance.

## Real-data design

The street graph is downloaded from OpenStreetMap at runtime. The repository does not commit the full Manhattan network because the generated node/edge files can be large and become stale. The included delivery file is a simulated business scenario placed at real Manhattan landmarks; it is used to exercise the optimization engine on the real road graph.

## Why C++

The project intentionally puts the computational core in C++ to demonstrate algorithm implementation, STL usage, graph modeling, OOP, memory-conscious data structures, compilation with CMake, and automated tests. Python is used only as a small ingestion utility for converting public OSM JSON into CSV files that the C++ program consumes.
