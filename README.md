
# EL PROYECTO SE ENCUENTRA EN LA RAMA MASTER
# Ecosystem Simulation: Sequential & Parallel Implementation

## Overview
Multi-agent ecosystem simulator featuring rabbits, foxes, and rocks on a discrete grid. Implements deterministic rules for movement, reproduction, and survival across multiple generations. Includes both sequential (C++) and parallel (OpenMP) versions, plus an interactive web visualization.

## Features
- **Deterministic Simulation**: Cell selection using `(gen + x + y) % P` formula ensures reproducible results
- **Complex Interactions**: Predator-prey dynamics with hunger, aging, and reproduction mechanics
- **Parallel Performance**: OpenMP implementation with strategic parallelization of planning phases
- **Web Visualization**: Real-time HTML5/JavaScript interface with population charts

## Project Structure
```
proyecto_visual/
├── ecosystem_simulation.cpp         # Sequential C++ implementation
├── ecosystem_simulation_omp.cpp     # Parallel OpenMP version
├── ecosystem.html                   # Web-based visualization
├── reporte_ecosistema.tex           # Technical report (LaTeX)
├── test_input.txt                   # Sample input file
└── README.md                        # This file
```

## Compilation

### Sequential Version
```bash
# Windows (MinGW-w64)
g++ -O2 -std=c++11 -o ecosystem ecosystem_simulation.cpp

# Linux / macOS
g++ -O2 -std=c++11 ecosystem_simulation.cpp -o ecosystem
```

### Parallel Version (OpenMP)
```bash
# Windows (MinGW-w64)
g++ -O2 -fopenmp -std=c++17 -o ecosystem_omp ecosystem_simulation_omp.cpp

# Linux / macOS
g++ -O2 -fopenmp -std=c++17 ecosystem_simulation_omp.cpp -o ecosystem_omp
```

## Execution

### Sequential
```bash
./ecosystem < test_input.txt
```

### Parallel (specify thread count)
```bash
# Windows
set OMP_NUM_THREADS=4
ecosystem_omp.exe < test_input.txt

# Linux / macOS
export OMP_NUM_THREADS=4
./ecosystem_omp < test_input.txt
```

### Web Visualization
Open `ecosystem.html` in a modern browser (Chrome, Firefox, Edge). Paste input data and click "Load/Reset Simulation".

## Input Format
```
GEN_PROC_RABBITS GEN_PROC_FOXES GEN_FOOD_FOXES N_GEN R C N
TYPE ROW COL
TYPE ROW COL
...
```

**Parameters:**
- `GEN_PROC_RABBITS`: Generations before rabbit can reproduce
- `GEN_PROC_FOXES`: Generations before fox can reproduce
- `GEN_FOOD_FOXES`: Generations fox can survive without food
- `N_GEN`: Number of generations to simulate
- `R`, `C`: Grid dimensions (rows, columns)
- `N`: Number of initial entities

**Entity Types:** `ROCK`, `RABBIT`, `FOX`

### Example Input
```
2 4 3 6 5 5 9
ROCK 0 0
RABBIT 0 2
FOX 0 4
FOX 1 0
FOX 1 4
ROCK 2 4
RABBIT 3 0
RABBIT 4 0
FOX 4 4
```

## Output Format
```
GEN_PROC_RABBITS GEN_PROC_FOXES GEN_FOOD_FOXES 0 R C COUNT
TYPE ROW COL
TYPE ROW COL
...
```
Final state lists all surviving entities and rocks.

## Simulation Rules

### Movement
- **Direction Priority**: North, East, South, West (clockwise)
- **Selection**: `(generation + x + y) % available_cells`
- Animals select from adjacent empty cells; stay if none available

### Reproduction
- **Rabbits**: Create offspring at original position when `proc_age >= GEN_PROC_RABBITS` and successfully move
- **Foxes**: Same mechanism with `GEN_PROC_FOXES` threshold
- Offspring appear immediately but don't act until next generation
- Parent's age resets to 0 after reproduction

### Fox Survival
- Hunger increments each generation
- Dies if `hunger >= GEN_FOOD_FOXES` and no adjacent rabbit available
- Hunger resets to 0 upon eating a rabbit

### Conflict Resolution
- Multiple entities targeting same cell: highest `proc_age` wins
- Ties broken by lowest `hunger` (foxes only)
- Losers are removed from simulation

## Performance Notes
- **Sequential**: Baseline reference implementation
- **Parallel**: Gains scale with population size and core count
- Typical speedup: 2-4x on quad-core systems with large populations
- Timing output includes total simulation time in seconds

## Technical Details
- **Language**: C++11 (sequential), C++17 (parallel)
- **Parallelization**: OpenMP 4.5+
- **Data Structures**: Separate vectors for rabbits, foxes; 2D grid for fast lookups
- **Web Stack**: Vanilla JavaScript (ES6), HTML5 Canvas

## Documentation
Full algorithm description, parallelization strategy, and analysis available in `reporte_ecosistema.tex`. Compile with:
```bash
pdflatex reporte_ecosistema.tex
```

## License
Academic project - Universidad, Parallel Computing Course, Semester 7, 2025.

## Authors
Julian Galviz Morales  - Implementation and documentation
Rui Yu Lei Wu
Benjamin Ortiz Morales
Marco Antonio Riascos Salguero 
