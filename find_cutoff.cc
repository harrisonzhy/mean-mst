#include "setup.hh"

#define PERCENTILE 0.999
#define DEF_TRIALS 100000
unsigned int NTRIALS = DEF_TRIALS;

void cutoffdn (unsigned int NDIM) {
    NTRIALS = DEF_TRIALS;
    auto q = 65536;
    for (; q <= 262144; q << 1) {
        std::vector<float> total;
        if (q >= 131072) {
            NTRIALS = 10;
        }
        total.reserve(NTRIALS);
        for (auto i = 0; i<NTRIALS; ++i) {
            // get random coord
            std::array<float, 4> fixedcoord;
            for (auto j = 0; j < NDIM; ++j) {
                fixedcoord[j] = getrandnum();
            }
            // compares squared values
            float min = 1.0 * NDIM;
            for (auto k = 0; k < q; ++k) {
                float gen = 0.0;
                // generate random euclidean distances from `fixedcoord`
                for (auto d = 0; d < NDIM; ++d) {
                    float r = getrandnum();
                    gen += (fixedcoord[d] - r) * (fixedcoord[d] - r);
                }
                if (gen < min) {
                    min = gen;
                }
            }
            total.emplace_back(min);
        }
        std::sort(total.begin(), total.begin() + NTRIALS);
        printf("%.4fth percentile element for DIM=%d, NEDGES=%d is: %.7f\n", PERCENTILE, NDIM, q, total[PERCENTILE*NTRIALS]);
    }
    printf("\n");
}

int main () {
    for (auto i = 2; i <= 4; ++i) {
        cutoffdn(i);
        return 0;
    }
}
