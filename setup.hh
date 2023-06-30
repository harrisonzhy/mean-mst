#pragma once
#include <random>
#include <execution>
#include <algorithm>
#include <thread>
#include <array>
#include <iostream>
#include <vector>
#include <time.h>
#include <chrono>
#include <random>
#include <iostream>

#define NTHREADS 16

auto getrandnum () -> float const {
    return (float)rand() / RAND_MAX;
}

template<size_t S>
class PointSOA {
    public :
        PointSOA (size_t s) {
            coords.resize(s);
            rank.resize(s);
            parent.resize(s);
        }
        PointSOA () {}

        void genpoints (size_t s) {
            for (auto i = 0; i < s; ++i) {
                parent[i] = i;
                rank[i] = 0;
                for (auto j = 0; j < S; ++j) {
                    coords[i][j] = getrandnum();
                }
                for (auto j = S; j < 4; ++j) {
                    coords[i][j] = 0;
                }
            }
        }

        void reset () {
            next = 0;
        }

        void resize(size_t s) {
            coords.resize(s);
            rank.resize(s);
            parent.resize(s);
        }

        void push (std::array<float,4> const& coord, unsigned r, unsigned p) {
            coords[next] = coord;
            rank[next] = r;
            parent[next] = p;
            ++next;
        }

        // auto-vectorize (SIMD) by setting S = 4
        std::vector<std::array<float,4>> coords;
        std::vector<unsigned> rank;
        std::vector<unsigned> parent;
        unsigned next = 0;
};

template<size_t S>
class EdgeSOA {
    public :
        EdgeSOA (size_t s) {
            p1.reserve(s);
            p2.reserve(s);
            weight.reserve(s);
        }
        std::vector<int> p1;
        std::vector<int> p2;
        std::vector<float> weight;
};

template<size_t S>
class Edge {
    public :
        int p1;
        int p2;
        float weight;
};

template<size_t S>
class GraphSOA {
    public :
        PointSOA<S> vertices;
        std::vector<Edge<S>> edges;
        size_t numpoints;
        std::atomic<int> next;

        GraphSOA (size_t s, size_t max) :
            vertices(max), edges(max*(max-1)/100), numpoints(s) {
                vertices.genpoints(s);
        }

        void linkgraph (int start, int end, const float MAX_WEIGHT) {
            auto nextin = next.fetch_add(1,std::memory_order_relaxed);
            for (auto i = start; i < end; ++i) {
                for (auto j = i + 1; j < numpoints; ++j) {
                    auto w = getedgew(i, j);
                    if (w < MAX_WEIGHT) [[unlikely]] {
                        edges[nextin].p1 = i;
                        edges[nextin].p2 = j;
                        edges[nextin].weight = w;
                        nextin = next.fetch_add(1, std::memory_order_relaxed);
                    }
                }
            }
        }

        auto getedgew (int p1, int p2) -> float {
            float w = 0.0;
            auto d1 = vertices.coords[p1][0] - vertices.coords[p2][0];
            auto d2 = vertices.coords[p1][1] - vertices.coords[p2][1];
            auto d3 = vertices.coords[p1][2] - vertices.coords[p2][2];
            auto d4 = vertices.coords[p1][3] - vertices.coords[p2][3];
            w += d1 * d1;
            w += d2 * d2;
            w += d3 * d3;
            w += d4 * d4;
            return w;
        }

        auto getmaxw () -> float {
            // hard-coded functions determined by `findcutoff.cc'
            switch(S) {
                case 1 : {
                    return 30.8 * pow(numpoints, float(-1.13));
                }
                case 2 : {
                    return 4.25 * pow(numpoints, float(-0.779));
                }
                case 3 : {
                    return 4.85 * pow(numpoints, float(-0.75));
                }
                case 4 : {
                    return 5.36 * pow(numpoints, float(-0.579));
                }
                default : {
                    // should not get here!
                    assert(false);
                }
            }
        }

        void createthreads () {
            std::vector<std::thread> threads;
            threads.reserve(NTHREADS);
            auto const numedges = numpoints * (numpoints - 1) / 2;
            auto const ratio = numedges / NTHREADS;
            float const MAXWEIGHT = getmaxw();
            auto edgestoadd = numpoints - 1;
            auto start = 0;
            auto end = 0;
            for (auto i = 0; i < NTHREADS; ++i) {
                auto edgecount = 0;
                while (edgecount < ratio && edgestoadd > 0) {
                    ++end;
                    edgecount += edgestoadd;
                    --edgestoadd;
                }
                threads.emplace_back(std::thread(&GraphSOA::linkgraph, this, start, end, MAXWEIGHT));
                start = end;
            }
            for (auto &t : threads) {
                try {
                    t.join();
                }
                catch (...) {
                    printf("EXCEPTION CAUGHT: std::thread.join()\n");
                }
            }
        }

        void sortedgew() {
            int s = next;
            std::sort(std::execution::par_unseq, edges.begin() + 1, edges.begin() + s,
                [](auto i, auto j) -> bool {
                    return i.weight < j.weight;
                });
        }

        void resize (size_t s) {
            numpoints = s;
        }

        void reset () {
            next = 0;
            vertices.genpoints(numpoints);
        }
};
