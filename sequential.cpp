#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>

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

// Hash function for unordered_map keys (for 3D coordinates)
struct Hash
{
    size_t operator()(const tuple<int, int, int> &key) const
    {
        return get<0>(key) * 73856093 ^ get<1>(key) * 19349663 ^ get<2>(key) * 83492791;
    }
};

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

    // Write output file
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

int main(int argc, char *argv[])
{
    // Command-line argument parsing
    string inputFile, outputFile;
    bool flag3D = false;

    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "--flag3d" && i + 1 < argc)
        {
            flag3D = stoi(argv[++i]) != 0;
        }
        else if (arg == "--input" && i + 1 < argc)
        {
            inputFile = argv[++i];
        }
        else if (arg == "--output" && i + 1 < argc)
        {
            outputFile = argv[++i];
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
    dijkstra(X, Y, Z, flag3D, Sx, Sy, Sz, edges, outputFile);

    cout << "SSSP results written to " << outputFile << "\n";
    return 0;
}
