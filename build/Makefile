CC = g++
OPT = -O3
#OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List corresponding compiled object files here (.o files)
SIM_OBJ = main.o cache.o cpu.o common.o
 
#################################

# default rule

all: cache_sim
	@echo "my work is done here..."


# rule for making cache_sim

sim_cache: $(SIM_OBJ)
	$(CC) -o cache_sim $(CFLAGS) $(SIM_OBJ) -lm
	@echo "-----------DONE WITH CACHE_SIM-----------"


# generic rule for converting any .cc file to any .o file
%.o: ../src/%.cpp
	$(CC) $(CFLAGS) -I../src/ -c $^


# type "make clean" to remove all .o files plus the cache_sim binary

clean:
	rm -f *.o cache_sim


# type "make clobber" to remove all .o files (leaves cache_sim binary)

clobber:
	rm -f *.o
