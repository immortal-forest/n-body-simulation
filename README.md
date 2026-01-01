# Benchmark

This branch focuses on benchmarking the performance of N-Body gravity simulations across different implementation methods: **Serial C**, **Parallel C (OpenMP)**, and **CUDA (NVIDIA GPU)**.

It includes a Python automation script to run tests with varying particle counts and visualize the speedup achieved by parallelization and GPU acceleration.

## Project Overview

The project implements the same core N-Body physics logic in three ways:

1. **Serial**: Standard C implementation running on a single CPU core.
1. **Parallel**: Multi-threaded C implementation using OpenMP.
1. **CUDA**: Massively parallel implementation running on NVIDIA GPUs.

## Prerequisites

To compile and run the benchmarks, you will need:

- **GCC**: With OpenMP support (usually built-in).
- **NVCC**: The NVIDIA CUDA Compiler (part of the CUDA Toolkit).
- **Python 3.x**: For running the benchmark runner.
- **Matplotlib**: For plotting results.

Install Python dependencies:

```bash
uv add matplotlib

```

## Compilation

Use the provided `Makefile` to compile all three versions of the simulation.

```bash
make

```

This will create a `build/` directory containing three executables:

- `build/serial`
- `build/parallel`
- `build/cuda`

*Note: The `clean` target can be used to remove build artifacts: `make clean`.*

## Running the Benchmark

The benchmarking logic is contained in `app/benchmark.py`. This script automatically:

1. Iterates through different body counts (e.g., 500, 1000, 2000, 5000, 10000).
1. Runs all three executables for a fixed number of steps (default: 100).
1. Measures execution time.
1. Generates a performance comparison plot.

To run the full benchmark suite:

```bash
uv run app/benchmark.py

```

### Configuration

You can adjust the test parameters by editing `app/benchmark.py`:

- `BODY_COUNTS`: List of particle counts to test.
- `STEPS`: Number of simulation steps per test (keep this low for benchmarking to save time).

## Results

After the script finishes, it will print a timing summary to the console and save a chart to the project root:

- **Console Output**: Table showing `Bodies`, `Type`, and `Time (s)`.
- **Chart**: `benchmark_results.png` (Log-scale plot comparing execution times).

## Project Structure

```text
galaxy-sim/
├── app/
│   └── benchmark.py      # Automation script for running tests & plotting
├── native/
│   ├── main.c            # Serial implementation source
│   ├── mainp.c           # OpenMP Parallel implementation source
│   ├── main.cu           # CUDA GPU implementation source
│   ├── physics.c         # Shared physics logic (CPU)
│   └── types.h           # Shared data structures
├── Makefile              # Build instructions for all targets
└── README.md             # This file

```
