# High-Performance Galaxy Simulation (N-Body)

A real-time N-Body gravity simulation engine built with **C (OpenMP)** and visualized using **Python (PyGame)**.

This project demonstrates two advanced methods of High-Performance Computing (HPC) and Inter-Process Communication (IPC):

1. **Shared Memory (IPC):** A standalone C engine writes directly to RAM, and Python reads it live.
1. **Shared Library (FFI):** Python loads the C engine as a plugin (`.so`) and drives the simulation step-by-step.

## Features

- **Physics Engine:** $O(N^2)$ Gravitational N-Body algorithm written in C.
- **Parallelism:** Uses OpenMP to utilize all CPU cores.
- **Visualization:** Real-time rendering with PyGame.
- **Architecture:**
  - Zero-Copy data transfer using `mmap` / POSIX Shared Memory.
  - Direct memory access using `ctypes` (Foreign Function Interface).

______________________________________________________________________

## Prerequisites

- **GCC** (with OpenMP support)
- **Make**
- **UV**

## Setup

1. **Initialize Python Environment**
   We use `uv` for dependency management.

   ```bash
   uv init
   uv add pygame numpy
   ```

1. **Compile the C Engine**
   Use the Makefile to build the shared memory executable and the shared library.

   ```bash
   # clean old builds
   make clean

   # shared memory executable (build/shared_galaxy)
   make shared_mem

   # shared library (build/libgalaxy.so)
   make library
   ```

______________________________________________________________________

## Shared Memory (IPC)

In this mode, the C program runs independently in its own process. It writes positions to a reserved block of RAM (`/dev/shm/shared_galaxy`). The Python script merely attaches to that memory to draw.

**1. Start the Physics Engine (Terminal 1)**

```bash
# Usage: ./build/shared_galaxy [num_bodies] [num_steps]
./build/shared_galaxy 2000 50000

```

*The simulation is now running in the background.*

**2. Start the Visualizer (Terminal 2)**

```bash
uv run app/visualize_shared.py

```

*You should see the window open. If you close the C program, the Python window stops updating.*

______________________________________________________________________

## Shared Library (FFI)

In this mode, Python is the "Master". It loads the compiled C code as a library. Python decides when to calculate the next frame.

**1. Run the Python Driver**

```bash
# Ensure you ran 'make library' first!
uv run app/visualize_library.py

```

*Python will initialize the C engine, run the loop, and handle the window all in one command.*

______________________________________________________________________

## Project Structure

```text
.
├── app/
│   ├── __init__.py
│   ├── visualize.py            # Basic CSV visualizer
│   ├── visualize_library.py    # Driver for Shared Library Mode
│   └── visualize_shared.py     # Viewer for Shared Memory Mode
├── data/
│   └── output.csv              # Output from basic main.c runs
├── native/
│   ├── main.c                  # Basic CLI version (writes to CSV)
│   ├── shared.c                # Entry point for Shared Memory Mode (IPC)
│   ├── library.c               # Entry point for Shared Library Mode (FFI)
│   ├── galaxy.c                # Galaxy initialization (Random distribution)
│   ├── physics.c               # Physics engine (Force & Integration)
│   ├── types.h                 # Struct definitions (Body, Vector)
│   ├── shared.h                # Shared Memory protocol definition
│   ├── galaxy.h                # Function prototypes
│   └── physics.h               # Physics prototypes
├── Makefile                    # Build scripts
├── pyproject.toml              # uv dependency file
├── uv.lock                     # Locked dependencies
└── README.md

```

## Performance Notes

- **Parallelism**: The engine uses `#pragma omp parallel` to utilize all CPU cores.
- **Race Conditions:** The physics loop uses a "Brute Force" parallel approach to ensure thread safety without locks.
- **Memory:** Shared Memory uses POSIX `shm_open`. The program automatically unlinks memory on exit, but if it crashes, you may need to verify `/dev/shm/` is clean.
