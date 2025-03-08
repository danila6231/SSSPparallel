import subprocess
import time
import matplotlib.pyplot as plt
import os

# File paths
executable = "./main"
# input_file = "tests/input_2d_uniform.txt"
log_file = "logs.txt"
threads_range = range(1, 9)  # 1 to 8 threads
output_dir = "./outputs"
graphs_dir = "./graphs"
os.makedirs(output_dir, exist_ok=True)
os.makedirs(graphs_dir, exist_ok=True)
output_correct = f"{output_dir}/output_correct.txt"

def run_analysis(name, input_file, graph_prefix, flag3d):
    print(f"===============RUNNING ANALYSIS FOR {name} CASE===============")
    print("Running Dijkstra's algorithm to get the correct output")
    subprocess.run([executable, "--algo", "dijkstra", "--flag3d", str(flag3d),
                    "--input", input_file, "--output", output_correct])
    # Step 2: Sort output_correct.txt
    with open(output_correct, "r") as f:
        sorted_correct = sorted(f.readlines())

    with open(output_correct, "w") as f:
        f.writelines(sorted_correct)

    # Step 3: Run Bellman-Ford with varying threads and collect data
    execution_times = []
    iterations_per_thread = []

    for n in threads_range:
        output_n = f"{output_dir}/output_{n}.txt"
        
        # Time execution
        start_time = time.time()
        print(f"Running Bellman-Ford for {n} threads")
        subprocess.run([executable, "--algo", "bellmanford", "--nthreads", str(n),
                        "--flag3d", str(flag3d), "--input", input_file, "--output", output_n])
        exec_time = time.time() - start_time
        execution_times.append(exec_time)
        print(f"Run finished in {exec_time}")

        # Read iterations from logs.txt
        with open(log_file, "r") as log_f:
            iterations = int(log_f.readline().strip())
            iterations_per_thread.append(iterations)

        # Step 3d: Check correctness by comparing sorted outputs
        with open(output_n, "r") as f:
            sorted_output_n = sorted(f.readlines())

        with open(output_n, "w") as f:
            f.writelines(sorted_output_n)

        if sorted_output_n != sorted_correct:
            print(f"❌ Bellman-Ford with {n} threads produced incorrect output!")
            exit(1)
    print(f"✅ Bellman-Ford produced correct output in all cases!")

    # Generate plots

    # Plot time vs. number of threads
    plt.figure()
    plt.plot(threads_range, execution_times, marker='o', linestyle='-')
    plt.xlabel("Number of Threads")
    plt.ylabel("Execution Time (seconds)")
    plt.title("Execution Time vs. Number of Threads")
    plt.grid()
    plt.savefig(f"{graphs_dir}/{graph_prefix}_execution_time_vs_threads.png")
    # plt.show()

    # Plot iterations vs. number of threads
    plt.figure()
    plt.plot(threads_range, iterations_per_thread, marker='s', linestyle='--', color='r')
    plt.xlabel("Number of Threads")
    plt.ylabel("Number of Iterations")
    plt.title("Iterations vs. Number of Threads")
    plt.grid()
    plt.savefig(f"./{graphs_dir}/{graph_prefix}_iterations_vs_threads.png")
    # plt.show()
    print(f"===============ANALYSIS FOR {name} CASE FINISHED===============")

run_analysis(
    name="2D UNIFORM WEIGHTS CASE",
    input_file="tests/input_2d_uniform.txt",
    graph_prefix="2d_uniform",
    flag3d=0
)

run_analysis(
    name="2D RANDOM WEIGHTS CASE",
    input_file="tests/input_2d_random.txt",
    graph_prefix="2d_random",
    flag3d=0
)

run_analysis(
    name="3D UNIFORM WEIGHTS CASE",
    input_file="tests/input_3d_uniform.txt",
    graph_prefix="3d_uniform",
    flag3d=1
)

run_analysis(
    name="3D RANDOM WEIGHTS CASE",
    input_file="tests/input_3d_random.txt",
    graph_prefix="3d_random",
    flag3d=1
)
