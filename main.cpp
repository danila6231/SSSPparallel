#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <omp.h>

using namespace std;
const int INF = INT_MAX;

struct Node
{
    int x, y, z;
    double dist;
    bool operator>(const Node &other) const { return dist > other.dist; }
};

struct Edge
{
    int x1, y1, z1, x2, y2, z2;
    double weight;
};

// hash function for unordered_map keys
struct Hash
{
    size_t operator()(const tuple<int, int, int> &key) const
    {
        return get<0>(key) * 73856093 ^ get<1>(key) * 19349663 ^ get<2>(key) * 83492791;
    }
};

void write_output(const unordered_map<tuple<int, int, int>, double, Hash>& dist, int flag3D, string outputFile) {
    ofstream outFile(outputFile);
    for (auto &[key, distance] : dist)
    {
        if (distance == INF)
            continue; // Ignore unreachable nodes
        outFile << get<0>(key) << " " << get<1>(key);
        if (flag3D)
            outFile << " " << get<2>(key);
        outFile << " " << distance << "\n";
    }
    outFile.close();
}

void dijkstra(int X, int Y, int Z, int flag3D, int Sx, int Sy, int Sz,
              const vector<Edge> &edges, string outputFile)
{
    unordered_map<tuple<int, int, int>, double, Hash> dist;
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    // Initialize distances
    for (int x = 0; x < X; ++x)
        for (int y = 0; y < Y; ++y)
            for (int z = 0; z < (flag3D ? Z : 1); ++z)
                dist[{x, y, z}] = INF;

    dist[{Sx, Sy, Sz}] = 0;
    pq.push({Sx, Sy, Sz, 0});

    // Adjacency list
    unordered_map<tuple<int, int, int>, vector<pair<tuple<int, int, int>, double>>, Hash> graph;
    for (const auto &e : edges)
    {
        graph[make_tuple(e.x1, e.y1, e.z1)].emplace_back(make_tuple(e.x2, e.y2, e.z2), e.weight);
        graph[make_tuple(e.x1, e.y1, e.z1)].emplace_back(make_tuple(e.x1, e.y1, e.z1), e.weight);
    }

    // Dijkstra's Algorithm
    while (!pq.empty())
    {
        Node curr = pq.top();
        pq.pop();

        if (curr.dist > dist[{curr.x, curr.y, curr.z}])
            continue;

        for (auto &[neighbor, weight] : graph[{curr.x, curr.y, curr.z}])
        {
            double newDist = curr.dist + weight;
            if (newDist < dist[neighbor])
            {
                dist[neighbor] = newDist;
                pq.push({get<0>(neighbor), get<1>(neighbor), get<2>(neighbor), newDist});
            }
        }
    }

    write_output(dist, flag3D, outputFile);
}

void bellman_ford(int X, int Y, int Z, int flag3D, int Sx, int Sy, int Sz,
                  const vector<Edge> &edges, string outputFile, int n_threads)
{
    omp_set_dynamic(0);     // Explicitly disable dynamic teams
    omp_set_num_threads(n_threads);
    using Coord = tuple<int, int, int>;

    unordered_map<Coord, double, Hash> dist;

    // Initialize distances to infinity
    for (int x = 0; x < X; ++x)
        for (int y = 0; y < Y; ++y)
            for (int z = 0; z < (flag3D ? Z : 1); ++z)
                dist[{x, y, z}] = INF;

    dist[{Sx, Sy, Sz}] = 0.0; // Source vertex distance is 0

    int V = X * Y * (flag3D ? Z : 1);
    int iterations = 0;
    // Bellman-Ford relaxation steps
    for (int i = 0; i < V - 1; ++i)
    {
        bool changed = false;
  #pragma omp parallel for shared(dist) reduction(|| : changed)
        for (size_t j = 0; j < edges.size(); ++j)
        {
            auto &[x1, y1, z1, x2, y2, z2, weight] = edges[j];

            Coord u = make_tuple(x1, y1, z1);
            Coord v = make_tuple(x2, y2, z2);

            if (dist[u] != INF && dist[u] + weight < dist[v])
            {
                iterations++;
  #pragma omp critical
                {
                    dist[v] = dist[u] + weight;
                    changed = true;
                }
            }
        }

        if (!changed)
            break; // Early exit if no updates
    }

    write_output(dist, flag3D, outputFile);

    // Write logs with num of iterations
    ofstream logFile("logs.txt");
    logFile << iterations <<  "\n";
    logFile.close();
}

int main(int argc, char *argv[])
{
    // Command-line argument parsing
    string inputFile, outputFile;
    bool flag3D = false;
    string algo = "dijkstra";
    int n_threads = omp_get_num_threads();

    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "--flag3d" && i + 1 < argc)
        {
            flag3D = stoi(argv[++i]) != 0;
        }
        else if (arg == "--algo" && i + 1 < argc)
        {
            algo = argv[++i];
        }
        else if (arg == "--input" && i + 1 < argc)
        {
            inputFile = argv[++i];
        }
        else if (arg == "--output" && i + 1 < argc)
        {
            outputFile = argv[++i];
        }
        else if (arg == "--nthreads" && i + 1 < argc)
        {
            n_threads = stoi(argv[++i]);
        }
    }

    // Read input file
    ifstream inFile(inputFile);
    if (!inFile)
    {
        cerr << "Error opening input file!\n";
        return 1;
    }

    int X, Y, Z = 1, Sx, Sy, Sz = 0;
    inFile >> X >> Y;
    if (flag3D)
        inFile >> Z;
    inFile >> Sx >> Sy;
    if (flag3D)
        inFile >> Sz;

    vector<Edge> edges;
    int x1, y1, z1 = 0, x2, y2, z2 = 0;
    double weight;
    while (inFile >> x1 >> y1)
    {
        if (flag3D)
            inFile >> z1;
        inFile >> x2 >> y2;
        if (flag3D)
            inFile >> z2;
        inFile >> weight;
        edges.push_back({x1, y1, z1, x2, y2, z2, weight});
    }
    inFile.close();

    // Run Dijkstra and write results
    if (algo == "dijkstra") {
        dijkstra(X, Y, Z, flag3D, Sx, Sy, Sz, edges, outputFile);
    }
    else {
        bellman_ford(X, Y, Z, flag3D, Sx, Sy, Sz, edges, outputFile, n_threads);
    }
    

    cout << "SSSP results written to " << outputFile << "\n";
    return 0;
}
