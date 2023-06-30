#pragma once
#include "setup.hh"

template<size_t S>
class KruskalSOA {
    public :
        static auto findset (GraphSOA<S> &G, int i) -> int {
            if (G.vertices.parent[i] != i) {
                G.vertices.parent[i] = findset(G,G.vertices.parent[i]);
            }
            return G.vertices.parent[i];
        }

        static void unionset (GraphSOA<S> &G, int p1, int p2) {
            if (G.vertices.rank[p1] < G.vertices.rank[p2]) {
                G.vertices.parent[p1] = p2;
            }
            else if (G.vertices.rank[p1] > G.vertices.rank[p2]) {
                G.vertices.parent[p2] = p1;
            }
            else {
                G.vertices.parent[p2] = p1;
                G.vertices.rank[p1]++;
            }
        }

        static auto createmst (GraphSOA<S> &G) -> float {
            float mincost = 0.0;
            int nedges = G.next;
            for (auto i = 0; i < nedges; ++i) {
                // different parents implies different sets
                auto par1 = findset(G, G.edges[i].p1);
                auto par2 = findset(G, G.edges[i].p2);
                if (par1 != par2) {
                    unionset(G, par1, par2);
                    mincost += sqrt(G.edges[i].weight);
                }
            }
            if (nedges < G.numpoints - 1) {
                mincost += (G.numpoints - 1 - nedges) * sqrt(G.getmaxw());
            }
            printf("MST Wt: %.6f | Vertices: %lu | Dim: %lu \n",
                mincost, G.numpoints, S);
            return mincost;
        }
};

template<size_t S>
void findmstSOA (int ntrials, int q) {
    GraphSOA<S> G(q, 262144);
    float avgw = 0.0;
    for (auto j = 0; j < ntrials; ++j) {
        auto const start = std::chrono::high_resolution_clock::now();
        G.createthreads();
        G.sortedgew();
        avgw += KruskalSOA<S>::createmst(G);
        auto const stop = std::chrono::high_resolution_clock::now();
        auto const duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        printf("Time(ms): %lld\n", duration.count());
        G.reset();
    }
    avgw /= ntrials;
    printf("\nAverage MST Wt: %.6f | Vertices: %d | Trials: %d | Dim: %lu\n\n",
        avgw, q, ntrials, S);
    G.reset();
}
