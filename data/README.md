# Data

## Real road-network data

`nodes.csv` and `edges.csv` are generated from **OpenStreetMap** through the Overpass API by running:

```bash
python scripts/download_osm.py
```

The downloader requests drivable highway ways inside a Manhattan bounding box, preserves one-way streets, computes segment lengths with the Haversine formula, and writes a directed graph consumed by the C++ engine.

The generated files are intentionally excluded from Git because a live OSM extract can be large and changes over time.

## Delivery scenario

`deliveries.csv` contains a simulated set of delivery requests at real Manhattan landmarks. These orders are not claimed to be historical company transactions; they are the business scenario used to test the optimization algorithms against the real road graph.

Columns:

- `name` — delivery location label
- `lat`, `lon` — geographic coordinate used to snap the stop to the nearest road node
- `priority` — relative delivery priority used by the route-seeding heuristic
