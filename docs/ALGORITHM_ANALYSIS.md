# Algorithm Analysis

## 1. Dijkstra's shortest path

For non-negative road weights, Dijkstra guarantees the optimal path.

Using an adjacency list and binary heap priority queue:

- Time: **O((V + E) log V)**
- Space: **O(V + E)**

This is the baseline used to validate shortest-path correctness.

## 2. A* search

A* uses:

`f(n) = g(n) + h(n)`

where `g(n)` is road distance traveled and `h(n)` is the Haversine straight-line distance to the target.

Because straight-line distance cannot exceed a valid road path between the same points, it is an admissible geographic heuristic for distance-based routing. A* therefore preserves shortest-path optimality while typically exploring a smaller part of the graph than uninformed Dijkstra.

Worst-case complexity is still comparable to Dijkstra, but practical search effort depends heavily on the quality of the heuristic.

The CLI reports both runtime and `expanded_nodes`, so the algorithms can be compared on the downloaded Manhattan graph rather than by theory alone.

## 3. Priority-aware nearest neighbor

For multiple deliveries, finding an exact optimal tour is related to the Traveling Salesperson Problem and becomes expensive as stop count grows. This project deliberately uses a heuristic approach suitable for a portfolio-scale routing engine.

At each step the scheduler evaluates currently unserved stops and chooses the best distance/priority score. This generates a fast initial route.

The heuristic does not claim global optimality.

## 4. 2-opt local search

The nearest-neighbor route is then improved with 2-opt. Two indices are selected and the segment between them is reversed. If the resulting route has a lower total shortest-path road distance, the candidate is accepted.

For `n` stops, one 2-opt pass evaluates **O(n²)** route modifications. Each candidate route currently recomputes shortest-path legs using A*, so this implementation favors clarity over maximum throughput.

A production version would cache pairwise shortest-path distances between depot/stops, reducing repeated graph searches.

## What is measured

The executable compares:

- Dijkstra distance
- Dijkstra expanded nodes
- Dijkstra runtime
- A* distance
- A* expanded nodes
- A* runtime
- baseline delivery route distance
- optimized delivery route distance
- percentage distance reduction

No benchmark values are hard-coded in the repository. Results depend on the current OpenStreetMap extract, compiler, machine, and delivery configuration.
