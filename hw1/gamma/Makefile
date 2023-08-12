CC := gcc
EXEC := qsort-mt
OBJS := xoroshiro128plus.o gamma_solution.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^

clean:
	$(RM) $(OBJS) $(EXEC)
