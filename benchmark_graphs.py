import csv
import re
from collections import defaultdict
from pathlib import Path

import matplotlib.pyplot as plt

CSV_FILE = Path("benchmark_results.csv")
OUTPUT_DIR = Path("benchmark_graphs")
OUTPUT_DIR.mkdir(exist_ok=True)

# Map the benchmark names in Google Benchmark's CSV to readable chart labels.
operation_names = {
    "Add": "Add",
    "Cancel": "Cancel",
    "GetBestBid": "Get Best Bid",
    "GetBestAsk": "Get Best Ask",
}
implementation_names = {
    "PriorityQueue": "Priority Queue",
    "Tick": "Tick",
    "Map": "Map",
}

# Store real_time (nanoseconds) by operation, implementation, and order count.
results = defaultdict(lambda: defaultdict(dict))

with CSV_FILE.open("r", newline="") as file:
    reader = csv.DictReader(file)
    for row in reader:
        name = row["name"]

        # Expected names look like BM_PriorityQueue_Add/100
        match = re.fullmatch(
            r"BM_(PriorityQueue|Tick|Map)_(Add|Cancel|GetBestBid|GetBestAsk)/(\d+)",
            name
        )
        if not match:
            continue

        implementation, operation, order_count = match.groups()
        results[operation][implementation][int(order_count)] = float(row["real_time"])

if not results:
    raise ValueError(
        "No benchmark rows were recognized. Make sure benchmark_results.csv "
        "is in the same folder as this script and has the Google Benchmark CSV format."
    )

for operation, chart_title in operation_names.items():
    if operation not in results:
        print(f"Skipping {chart_title}: no rows found.")
        continue

    fig, ax = plt.subplots(figsize=(8, 5))

    for implementation, legend_label in implementation_names.items():
        values = results[operation].get(implementation, {})
        if not values:
            continue

        x_values = sorted(values)
        y_values = [values[x] for x in x_values]
        ax.plot(x_values, y_values, marker="o", linewidth=2, label=legend_label)

    ax.set_title(f"{chart_title} Benchmark")
    ax.set_xlabel("Number of Orders")
    ax.set_ylabel("Execution Time (nanoseconds)")
    ax.set_xscale("log")
    if operation in ("Add", "Cancel"):
        # These operations span a wide range of times; log scale keeps all
        # three implementations visible on the same chart.
        ax.set_yscale("log")

    ax.set_xticks([100, 1000, 10000, 100000])
    ax.set_xticklabels(["100", "1,000", "10,000", "100,000"])
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    ax.legend()
    fig.tight_layout()

    output_file = OUTPUT_DIR / f"{operation}.png"
    fig.savefig(output_file, dpi=200)
    plt.close(fig)
    print(f"Saved {output_file}")

print("\nDone. Open the PNG files in the benchmark_graphs folder.")
print("Reminder: your pasted benchmark output reports that the benchmark library was built as DEBUG.")
