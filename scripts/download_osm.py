"""Download a real Manhattan drivable road graph from OpenStreetMap via Overpass.

Outputs:
  data/nodes.csv -> node_id,lat,lon
  data/edges.csv -> from,to,distance_m

The script queries highway ways inside a Manhattan bounding box and converts
OSM nodes/ways into a directed graph for the C++ engine.
"""

from __future__ import annotations

import csv
import math
from pathlib import Path
import requests

OVERPASS_URL = "https://overpass-api.de/api/interpreter"
# South, West, North, East: practical Manhattan bounding box
BBOX = (40.7003, -74.0200, 40.8820, -73.9070)
OUT_DIR = Path(__file__).resolve().parents[1] / "data"


def haversine_m(lat1: float, lon1: float, lat2: float, lon2: float) -> float:
    r = 6_371_000.0
    p1, p2 = math.radians(lat1), math.radians(lat2)
    dphi = math.radians(lat2 - lat1)
    dlambda = math.radians(lon2 - lon1)
    a = math.sin(dphi / 2) ** 2 + math.cos(p1) * math.cos(p2) * math.sin(dlambda / 2) ** 2
    return 2 * r * math.atan2(math.sqrt(a), math.sqrt(1 - a))


def main() -> None:
    south, west, north, east = BBOX
    query = f"""
    [out:json][timeout:120];
    way[highway][highway!~"footway|path|cycleway|steps|pedestrian|construction"]({south},{west},{north},{east});
    (._;>;);
    out body;
    """

    response = requests.post(OVERPASS_URL, data={"data": query}, timeout=180)
    response.raise_for_status()
    payload = response.json()

    nodes = {}
    ways = []
    for element in payload["elements"]:
        if element["type"] == "node":
            nodes[element["id"]] = (element["lat"], element["lon"])
        elif element["type"] == "way":
            ways.append(element)

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    with (OUT_DIR / "nodes.csv").open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["node_id", "lat", "lon"])
        for node_id, (lat, lon) in nodes.items():
            writer.writerow([node_id, lat, lon])

    edge_rows = []
    for way in ways:
        ids = way.get("nodes", [])
        tags = way.get("tags", {})
        oneway = tags.get("oneway") in {"yes", "1", "true"} or tags.get("junction") == "roundabout"
        for a, b in zip(ids, ids[1:]):
            if a not in nodes or b not in nodes:
                continue
            lat1, lon1 = nodes[a]
            lat2, lon2 = nodes[b]
            distance = haversine_m(lat1, lon1, lat2, lon2)
            edge_rows.append((a, b, distance))
            if not oneway:
                edge_rows.append((b, a, distance))

    with (OUT_DIR / "edges.csv").open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["from", "to", "distance_m"])
        writer.writerows(edge_rows)

    print(f"Downloaded {len(nodes):,} OSM nodes and wrote {len(edge_rows):,} directed edges.")


if __name__ == "__main__":
    main()
