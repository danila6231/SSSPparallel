import numpy as np
np.random.seed(1)

def generate_input_file(filename, grid_size, is_3d, uniform_weight=None, random_range=None, source=None):
    """
    Generates an input file for the Dijkstra's grid-based SSSP algorithm.
    
    :param filename: Name of the output file.
    :param grid_size: Tuple representing the size of the grid (X, Y) or (X, Y, Z) for 3D.
    :param is_3d: Boolean indicating if the grid is 3D.
    :param uniform_weight: Constant weight for all edges (if specified, random_range is ignored).
    :param random_range: Tuple (min, max) for random weights (if specified, uniform_weight is ignored).
    :param source: Tuple representing the source vertex (Sx, Sy) or (Sx, Sy, Sz).
    """
    X, Y = grid_size[:2]
    Z = grid_size[2] if is_3d else 1

    with open(filename, 'w') as f:
        # Write grid size
        if is_3d:
            f.write(f"{X} {Y} {Z}\n")
        else:
            f.write(f"{X} {Y}\n")

        # Write source vertex
        if is_3d:
            f.write(f"{source[0]} {source[1]} {source[2]}\n")
        else:
            f.write(f"{source[0]} {source[1]}\n")

        # Generate edges
        for x in range(X):
            for y in range(Y):
                for z in range(Z if is_3d else 1):
                    current = (x, y, z)

                    # Define neighbors based on grid connectivity
                    neighbors = []
                    if x + 1 < X: neighbors.append((x + 1, y, z))
                    if x - 1 >= 0: neighbors.append((x - 1, y, z))
                    if y + 1 < Y: neighbors.append((x, y + 1, z))
                    if y - 1 >= 0: neighbors.append((x, y - 1, z))
                    if is_3d:
                        if z + 1 < Z: neighbors.append((x, y, z + 1))
                        if z - 1 >= 0: neighbors.append((x, y, z - 1))

                    for neighbor in neighbors:
                        weight = uniform_weight if uniform_weight is not None else np.random.uniform(*random_range)
                        if is_3d:
                            f.write(f"{x} {y} {z} {neighbor[0]} {neighbor[1]} {neighbor[2]} {weight:.2f}\n")
                        else:
                            f.write(f"{x} {y} {neighbor[0]} {neighbor[1]} {weight:.2f}\n")

# Testcase 1: Uniform edge weights (1.0)
generate_input_file("./input_2d_uniform.txt", (1000, 1000), is_3d=False, uniform_weight=1.0, source=(0, 0))
generate_input_file("./input_3d_uniform.txt", (50, 50, 50), is_3d=True, uniform_weight=1.0, source=(0, 0, 0))

# Testcase 2: Random edge weights in range [1.0, 10.0]
generate_input_file("./input_2d_random.txt", (1000, 1000), is_3d=False, random_range=(1.0, 10.0), source=(500, 500))
generate_input_file("./input_3d_random.txt", (50, 50, 50), is_3d=True, random_range=(1.0, 10.0), source=(25, 25, 25))
