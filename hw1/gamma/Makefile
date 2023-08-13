CC := gcc
CFLAGS := -g
EXEC := qsort-mt
OBJS := xoroshiro128plus.o gamma_solution.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(OBJS) $(EXEC)
