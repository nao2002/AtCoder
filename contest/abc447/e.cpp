//abc447e
#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

void dfs(std::unordered_map<int, std::unordered_set<int>> &graph, std::unordered_set<int> &results, int pos) {
    results.insert(pos);

    for (const auto& vert: graph[pos]) {
        if (!results.contains(vert)) {
            dfs(graph, results, vert);
        }
    }
}

int main() {
    int N,M;
    std::cin >> N >> M;

    std::unordered_map<int, std::unordered_set<int>> graph {};
    std::unordered_map<int, std::unordered_set<int>> actualGraph {};

    std::vector<std::pair<int,int>> sides(M);
    // std::pair<int,int> sides[M] {};

    std::vector<bool> completeList(N);
    // bool completeList[N] {};

    for (int i=0;i<M;++i) {
        int U,V;
        std::cin >> U >> V;
        --U;
        --V;

        std::pair<int,int> side = {U,V};
        sides[i] = side;

        graph[U].insert(V);
        graph[V].insert(U);
        actualGraph[U].insert(V);
        actualGraph[V].insert(U);
    }

    int rootNode = -1;
    int lastRemovedSide = -1;

    for (int i=0;i<M;++i) {
        std::pair<int,int> side = sides[i];
        lastRemovedSide = i;

        actualGraph[side.first].erase(side.second);
        actualGraph[side.second].erase(side.first);

        if (graph[side.first].contains(side.second)) {

            graph[side.first].erase(side.second);
            graph[side.second].erase(side.first);

            // std::cout << "Current: " << side.first << side.second << "," << graph[side.first].size() << graph[side.second].size() << std::endl;

            if (graph[side.first].size() == 0) {
                rootNode = side.first;
                break;
            }
            if (graph[side.second].size() == 0) {
                rootNode = side.second;
                break;
            }

            if (graph[side.first].size() == 1) {
                for (const auto& s: graph[side.first]) {
                    if (completeList[s]) {
                        rootNode = side.first;
                    }
                    graph[s].erase(side.first);
                    break;
                }
                if (rootNode != -1) {
                    break;
                }
                graph[side.first].clear();
                completeList[side.first] = true;
            }

            if (graph[side.second].size() == 1) {
                for (const auto& s: graph[side.second]) {
                    if (completeList[s]) {
                        rootNode = side.second;
                    }
                    graph[s].erase(side.second);
                    break;
                }
                if (rootNode != -1) {
                    break;
                }
                graph[side.second].clear();
                completeList[side.second] = true;
            }
        } else {
            rootNode = side.first;
            break;
        }
    }

    // std::cout << lastRemovedSide << std::endl;

    std::unordered_set<int> connectedList = {};
    dfs(actualGraph,connectedList,rootNode);
    // std::cout << connectedList.size() << std::endl;

    // for (const auto& c: connectedList) {
    //     std::cout << "conn: " << c << std::endl;
    // }

    int ans = 0;
    const long long MOD = 998244353;

    std::vector<long long> cachedMOD(M);
    long long p = 0;
    for (int i=0;i<M;++i) {
        if (p != 0) {
            p = (p * 2) % MOD;
        } else {
            p = 2;
        }
        cachedMOD[i] = p;
    }

    for (int i=0;i<lastRemovedSide+1;++i) {
        std::pair<int,int> side = sides[i];
        if ((connectedList.contains(side.first) && !connectedList.contains(side.second)) || (!connectedList.contains(side.first) && connectedList.contains(side.second))) {
            ans += cachedMOD[i];
            ans %= MOD;
        }
    }

    ans %= MOD;

    std::cout << ans << std::endl;

    return 0;
}