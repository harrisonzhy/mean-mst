OBJ_NAME=randmst

CC=g++ -std=c++20

OBJS=randmst.cc

COMPILER_FLAGS=-Ofast -ffast-math

HEADERS=setup.hh kruskal.hh

# enable for faster sort with `std::execution::par_unseq' if possible
LINKER_FLAGS=-lttb

randmst : $(OBJS) $(HEADERS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o randmst

clean:
	$(info CLEAN) @rm *.o randmst
