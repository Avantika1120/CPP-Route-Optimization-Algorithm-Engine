# C++ Route Optimization & Algorithm Engine

**C++17 · Graph Algorithms · Dijkstra · A* · Greedy Heuristics · 2-Opt · STL · CMake · OpenStreetMap**

A real-data algorithmic-design project that uses a **live Manhattan road network from OpenStreetMap** to solve shortest-path and multi-stop delivery-routing problems in C++.

This project was built to revisit and demonstrate the C++/algorithmic-design foundation from my MCA while connecting it to a practical optimization problem.

## What I built

- Downloaded a **real Manhattan road graph** from OpenStreetMap using the Overpass API
- Converted OSM ways/nodes into a directed weighted graph
- Built the graph engine in **C++17** using adjacency lists and STL containers
- Implemented **Dijkstra's shortest-path algorithm** as a correctness baseline
- Implemented **A* search** using Haversine distance as a geographic heuristic
- Built a **priority-aware nearest-neighbor** heuristic for multi-stop delivery scheduling
- Improved the initial delivery sequence using **2-opt local search**
- Added runtime and node-expansion benchmarking for Dijkstra vs A*
- Added CMake builds, unit tests, CI, architecture documentation, and complexity analysis

## Why this problem?

Routing is a useful way to demonstrate algorithmic design because it combines:

- graph modeling
- priority queues
- shortest-path algorithms
- geospatial heuristics
- approximation/optimization techniques
- time/space complexity tradeoffs
- clean C++ data structures and modular design

Instead of running the algorithms on a hand-created classroom graph, this project uses actual public street-network data.

## Real data

### Road network

The road network comes from **OpenStreetMap (OSM)** through the public Overpass API.

Run:

```bash
python scripts/download_osm.py
```

The script downloads drivable Manhattan highway ways and produces:

```text
data/nodes.csv   -> node_id, latitude, longitude
data/edges.csv   -> from_node, to_node, road_segment_distance_m
```

One-way road information is preserved when creating directed edges.

### Delivery scenario

`data/deliveries.csv` is a simulated delivery workload placed at real Manhattan landmarks such as Grand Central, Penn Station, Union Square, Battery Park, and Columbus Circle.

The important distinction is:

- **road-network data = real OpenStreetMap data**
- **delivery requests = simulated business scenario on top of that real graph**

This keeps the project reproducible without pretending that the delivery orders are historical company records.

## End-to-end architecture

```text
OpenStreetMap / Overpass API
            |
            v
   Python ingestion utility
            |
            v
   nodes.csv + edges.csv
            |
            v
       C++ Graph Loader
            |
            v
   Adjacency-list Road Graph
            |
      +-----+------+
      |            |
      v            v
  Dijkstra        A*
      |            |
      +-----+------+
            |
            v
 Priority-aware Nearest Neighbor
            |
            v
        2-Opt Search
            |
            v
 Optimized Multi-stop Route
```

More detail: [Architecture](docs/ARCHITECTURE.md)

## Algorithms used

### 1. Dijkstra

Dijkstra is the baseline shortest-path algorithm for the non-negative road-distance graph.

With an adjacency list and binary heap:

**Time:** `O((V + E) log V)`  
**Space:** `O(V + E)`

It guarantees the optimal shortest-distance route.

### 2. A* search

A* adds a geographic heuristic:

```text
f(n) = g(n) + h(n)
```

where:

- `g(n)` = distance already traveled on the road graph
- `h(n)` = Haversine straight-line distance from the current node to the destination

The straight-line distance guides the search toward the target while retaining shortest-path correctness for distance-based routing.

The executable compares Dijkstra and A* using:

- route distance
- expanded nodes
- execution time

### 3. Priority-aware nearest neighbor

For a set of delivery stops, the engine creates a practical initial route by balancing road distance with delivery priority.

This is intentionally a heuristic rather than a claim of exact Traveling Salesperson optimization.

### 4. 2-opt local search

The initial route is improved by reversing route segments and keeping changes that reduce the total shortest-path road distance.

This demonstrates a common local-search optimization technique while keeping the implementation understandable and interview-friendly.

See [Algorithm Analysis](docs/ALGORITHM_ANALYSIS.md) for complexity and tradeoffs.

## Repository structure

```text
CPP-Route-Optimization-Algorithm-Engine/
├── include/
│   ├── Graph.hpp
│   ├── Algorithms.hpp
│   └── RouteOptimizer.hpp
├── src/
│   ├── Graph.cpp
│   ├── Algorithms.cpp
│   ├── RouteOptimizer.cpp
│   └── main.cpp
├── scripts/
│   └── download_osm.py
├── data/
│   ├── deliveries.csv
│   └── README.md
├── tests/
│   └── test_algorithms.cpp
├── docs/
│   ├── ARCHITECTURE.md
│   └── ALGORITHM_ANALYSIS.md
├── .github/workflows/
│   └── cpp.yml
├── CMakeLists.txt
├── requirements.txt
└── README.md
```

## Run the project

### 1. Download real OSM data

```bash
python -m venv .venv
source .venv/bin/activate     # Windows: .venv\Scripts\activate
pip install -r requirements.txt
python scripts/download_osm.py
```

### 2. Build the C++ project

```bash
cmake -S . -B build
cmake --build build --config Release
```

### 3. Run tests

```bash
ctest --test-dir build --output-on-failure
```

### 4. Run the optimizer

Linux/macOS:

```bash
./build/route_optimizer data/nodes.csv data/edges.csv data/deliveries.csv
```

Windows with a multi-config generator:

```powershell
.\build\Release\route_optimizer.exe data\nodes.csv data\edges.csv data\deliveries.csv
```

## What the program reports

The CLI prints:

- number of OSM nodes and directed edges loaded
- Dijkstra shortest-path distance
- Dijkstra nodes expanded and runtime
- A* shortest-path distance
- A* nodes expanded and runtime
- baseline delivery distance using input order
- optimized route distance
- percentage distance reduction
- final delivery-stop sequence

No benchmark values are hard-coded in this README because the road extract can change over time. The results are generated from the current OSM data when the project is run.

## C++ concepts demonstrated

- C++17
- classes and modular interfaces
- `std::unordered_map`
- `std::vector`
- `std::priority_queue`
- pairs and structured bindings
- adjacency-list graph representation
- exception handling
- file/CSV parsing
- algorithmic complexity analysis
- CMake
- unit testing
- GitHub Actions CI

## Key design decisions

**Why adjacency lists?**  
Street networks are sparse; storing only existing road connections is substantially more memory-efficient than a dense adjacency matrix.

**Why keep Python at all?**  
Python is used only as a lightweight ingestion utility for a public JSON API. The graph, shortest-path algorithms, route optimization, benchmarking, and core application logic are implemented in C++.

**Why not claim a globally optimal multi-stop route?**  
The multi-stop problem becomes computationally expensive as stop count grows. The project uses an explainable heuristic pipeline — nearest-neighbor seed followed by 2-opt — and clearly describes that tradeoff.

## Potential production improvements

- cache pairwise shortest-path distances between delivery stops
- use travel-time weights instead of only physical distance
- incorporate real traffic data
- add delivery time windows and vehicle capacities
- implement exact/branch-and-bound solving for small stop counts
- add contraction hierarchies or bidirectional search for faster city-scale routing
- visualize the final route on an interactive map

---

## Portfolio takeaway

This project demonstrates that I can move beyond scripting and analytics into **C++ algorithmic design**: modeling a real sparse road network, implementing shortest-path algorithms from first principles, designing optimization heuristics, analyzing complexity, and packaging the work as a testable software project.
