# Parallelized SSSP problem implementation using Bellman-Ford search and OpenMP

## Problem setting
Develop a parallel B-F SSSP program for 2D grids: X x Y and 3D grids: X x Y x Z.
Starting with 2D, each grid point (x,y) is a vertex and each vertex is connected by an edge to its 4 neighbors (equivalent to an 4-point stencil):

    (x,y,) <-> (x-1,y)
    
    (x,y) <-> (x+1,y)
    
    (x,y) <-> (x,y-1)
    
    (x,y) <-> (x,y+1)

In the case of 3D, each grid point (x,y,z) is a vertex and each vertex is connected by an edge to its 6 neighbors (equivalent to a 6-point stencil):

    (x,y,z) <-> (x-1,y,z)
    
    (x,y,z) <-> (x+1,y,z)
    
    (x,y,z) <-> (x,y-1,z)
    
    (x,y,z) <-> (x,y+1,z)
    
    (x,y,z) <-> (x,y,z-1)
    
    (x,y,z) <-> (x,y,z+1)

Each edge has a cost, c(e).  All of the edge costs need to be set to some value before the algorithm begins (see below).
Each vertex maintains its shortest path cost estimate and the neighbor it thinks is on the shortest path.
At the beginning, the cost estimate is infinity for all vertices except the source (which is 0).

Testcase 1:

  a. Use a 1000x1000 grid (2D) and 50x50x50 grid (3D).

  b. Set all edges in the grid to a uniform constant value (e.g., 1)

  c. Set the source to the grid point at (0,0) and (0,0,0)

  d. Time how long it takes to execute and the # iterations to complete.

Testcase 2:

  a. Use a 1000x1000 grid (2D) and 50x50x50 grid (3D).

  b. Set each edge in the grid to a random value in the range 1.0 to 10.0.

  c. Set the source to the grid point at the center (i.e., (500,500) for 2D and (25,25,25) for 3D.

  d. Time how long it takes to execute and the # iterations to complete.  Conduct a scaling study for different # threads.
  
## Implementation details
- gen_tests.py contains script to generate input files for corresponding test cases
    - 2d with random weights assignment
    - 2d with uniform weights assignment
    - 3d with random weights assignment
    - 3d with uniform weights assignment
- main.cpp
    - contains the implementation of the parallelized Bellman-Ford algorithm
        - Paralellization details:
            - OpenMP is used to distribute edge processing among different threads. This makes the process more efficient because in a lot of cases different edges can be processed independently.
            - We define a critical section for the part of code where we update the resulting distances to avoid data races.
    - contains implementation of serial Dijkstra algorithm to use as a benchmark for correct distances
    - accepts the folowing command line arguments
        - flag3d: whethere we are working with 2d or 3d grid
        - algo: which algorithm are we using (dijkstra / bellman ford)
        - input: input file address, input file contains size of grid, source vertex and edges weights
        - output: output, where we save the computed distances for each vertex
        - nthreads: number of threads to use in case of bellman ford
- main: compiled executable
- run.py: contains the analysis script

## Analysis
Ran the bellman ford implementation for all the test cases and number of threads in 1...8 range.

As the result in all cases the output was correct (the same as the Dijkstra algorithm).

The graphs displaying the execution times and iterations number depending on the number of threads can be found in the graphs folder.

The biggest speedup with increased number of threads can be noticed in case of randomized weights.

Here are the graphs for 3d randomized weights case:

![3d_random_execution_time_vs_threads](https://github.com/user-attachments/assets/032fea69-4446-401b-a14f-245d7f561706)

![3d_random_iterations_vs_threads](https://github.com/user-attachments/assets/71441ce3-e7a1-4d12-b63f-a97ede280843)

In case of constant weights we don't notice the similar speedup:

![3d_uniform_execution_time_vs_threads](https://github.com/user-attachments/assets/ebe35f89-2d7e-49d7-af91-1ce5a7ea6eaa)

![3d_uniform_iterations_vs_threads](https://github.com/user-attachments/assets/8275dca9-fbf0-4717-b806-ca832170dbe0)

## How to run
1. Compile main.cpp - may be different depending on the system and openmp location
```bash
g++ main.cpp --std=c++17 -o main -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp
```
2. Run the executable
```bash
./main --algo bellmanford --nthreads 8 --flag3d 0 --input tests/input_2d_uniform.txt --output output.txt
```

3. Run the script with analysis
```bash
python3 run.py
```
