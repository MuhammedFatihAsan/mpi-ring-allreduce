#!/usr/bin/env python3
# reads results/results.csv, computes per-config medians, dumps plots.
# usage: python3 scripts/parse_results.py

import csv
import sys
from collections import defaultdict
from pathlib import Path
from statistics import median

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

REPO = Path(__file__).resolve().parents[1]
CSV = REPO / "results" / "results.csv"
PLOTS = REPO / "results" / "plots"
PLOTS.mkdir(parents=True, exist_ok=True)

if not CSV.exists():
    print(f"missing {CSV} — run scripts/test_runner.sh first", file=sys.stderr)
    sys.exit(1)

# (algo, np, count) -> [comm_time_s, ...]
buckets = defaultdict(list)
with open(CSV) as f:
    for row in csv.DictReader(f):
        key = (row["algo"], int(row["np"]), int(row["count"]))
        buckets[key].append(float(row["comm_time_s"]))

med = {k: median(v) for k, v in buckets.items()}

all_np = sorted({k[1] for k in med})
all_counts = sorted({k[2] for k in med})

# 1) per-rank-count: time vs size, baseline vs ring
for np in all_np:
    base = [med.get(("baseline", np, c)) for c in all_counts]
    ring = [med.get(("ring", np, c)) for c in all_counts]

    fig, ax = plt.subplots(figsize=(8, 5))
    ax.plot(all_counts, base, "o--", label="baseline (Reduce+Bcast)")
    ax.plot(all_counts, ring, "s-", label="ring allreduce")
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlabel("array size (float elements)")
    ax.set_ylabel("median communication time (s)")
    ax.set_title(f"baseline vs ring, np = {np}")
    ax.grid(True, which="both", alpha=0.3)
    ax.legend()
    base = PLOTS / f"compare_np{np}"
    fig.tight_layout()
    fig.savefig(base.with_suffix(".png"), dpi=140)
    fig.savefig(base.with_suffix(".pdf"))
    plt.close(fig)
    print(f"wrote {base}.{{png,pdf}}")

# 2) speedup chart across all np
fig, ax = plt.subplots(figsize=(8, 5))
for np in all_np:
    spd = []
    for c in all_counts:
        b = med.get(("baseline", np, c))
        r = med.get(("ring", np, c))
        spd.append(b / r if (b and r) else float("nan"))
    ax.plot(all_counts, spd, "o-", label=f"np={np}")

ax.axhline(1.0, color="black", linestyle=":", alpha=0.6, label="break-even")
ax.set_xscale("log")
ax.set_xlabel("array size (float elements)")
ax.set_ylabel("speedup (baseline / ring)")
ax.set_title("ring speedup over baseline (>1 means ring wins)")
ax.grid(True, which="both", alpha=0.3)
ax.legend()
base = PLOTS / "speedup"
fig.tight_layout()
fig.savefig(base.with_suffix(".png"), dpi=140)
fig.savefig(base.with_suffix(".pdf"))
plt.close(fig)
print(f"wrote {base}.{{png,pdf}}")

# 3) print a quick summary table to stdout
print("\nmedian comm time (s):")
print(f"{'count':>10} | " + " | ".join(f"np={n} {a:<8}" for n in all_np for a in ("base", "ring")))
for c in all_counts:
    cells = []
    for n in all_np:
        b = med.get(("baseline", n, c))
        r = med.get(("ring", n, c))
        cells.append(f"{b:.5f}" if b else "    -   ")
        cells.append(f"{r:.5f}" if r else "    -   ")
    print(f"{c:>10} | " + " | ".join(cells))
