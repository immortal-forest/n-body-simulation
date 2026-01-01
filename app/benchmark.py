import os
import subprocess
import time

import matplotlib.pyplot as plt

# Configuration
BODY_COUNTS = [500, 1000, 2000, 5000, 10000]
STEPS = 100  # Keep steps low so slow tests don't take forever
EXECUTABLES = {
    "Serial C": "./build/serial",
    "Parallel C": "./build/parallel",
    "CUDA GPU": "./build/cuda",
}

results = {name: [] for name in EXECUTABLES}


def run_benchmark():
    print(f"{'Bodies':<10} | {'Type':<12} | {'Time (s)':<10}")
    print("-" * 36)

    for n in BODY_COUNTS:
        for name, exe in EXECUTABLES.items():
            # Skip Serial for huge numbers because it's too slow
            # if name == "Serial C" and n > 5000:
            # results[name].append(None)
            # continue

            start = time.time()
            # Run the command: ./exe [bodies] [steps]
            subprocess.run([exe, str(n), str(STEPS)], stdout=subprocess.DEVNULL)
            end = time.time()

            duration = end - start
            results[name].append(duration)

            print(f"{n:<10} | {name:<12} | {duration:.4f}")

    plot_results()


def plot_results():
    plt.figure(figsize=(10, 6))

    for name, times in results.items():
        # Filter out None values (skipped tests)
        valid_x = [BODY_COUNTS[i] for i, t in enumerate(times) if t is not None]
        valid_y = [t for t in times if t is not None]

        plt.plot(valid_x, valid_y, marker="o", label=name)

    plt.xlabel("Number of Bodies")
    plt.ylabel("Time (Seconds) - Lower is Better")
    plt.title("N-Body Benchmark: CPU vs GPU")
    plt.legend()
    plt.grid(True)
    plt.yscale("log")  # Log scale because GPU is usually insanely faster
    plt.savefig("benchmark_results.png")
    print("\nChart saved to benchmark_results.png")


if __name__ == "__main__":
    if not os.path.exists("build/cuda"):
        print("Error: Compile first! Run 'make'")
    else:
        run_benchmark()
