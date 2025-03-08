# Parallelized SSSP problem implementation using Bellman-Form search and OpenMP

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
