#include "setup.hh"
#include "kruskal.hh"

auto main (int argc, char** argv) -> int {
    if (argc < 5) {
    printf("%s %d\n", "INVALID INPUT: Need 5 args; have", argc);
        printf("%s\n", "USAGE: ./randmst 0 <numpoints> <numtrials> <dimension: 2,3,4>");
        exit(1);
    }
    if (atoi(argv[4]) == 1 || atoi(argv[4]) > 4) {
        printf("%s %d\n", "INVALID INPUT: Dimension %d is not supported", atoi(argv[4]));
        printf("%s\n", "USAGE: ./randmst 0 <numpoints> <numtrials> <dimension: 2,3,4>");
        exit(1);
    }
    srand((unsigned) time(NULL));

    // configurations
    auto nvertices = atoi(argv[2]);
    auto ntrials = atoi(argv[3]);
    auto dim = atoi(argv[4]);
    auto mode = atoi(argv[1]);
    if (mode == 0) {
        switch(dim) {
            case 2 : {
                findmstSOA<2>(ntrials, nvertices);
                break;
            }
            case 3 : {
                findmstSOA<3>(ntrials, nvertices);
                break;
            }
            case 4 : {
                findmstSOA<4>(ntrials, nvertices);
                break;
            }
            default : {
                printf("%s %d\n", "INVALID INPUT: Need valid dimension; have", atoi(argv[4]));
                printf("%s\n", "USAGE: ./randmst 0 <numpoints> <numtrials> <dimension: 2,3,4>");
                exit(1);
            }     
        }
    }
    return 0;
}
