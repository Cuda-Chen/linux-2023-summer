CFLAGS = -O2 -std=c11 -Wall -Wextra -I.. -MMD -DUSE_LINUX -D_GNU_SOURCE
CC = gcc
LDFLAGS = -lpthread -fsanitize=thread -fPIE -pie

OUT ?= build
BINARY = $(OUT)/priority-inversion

SHELL_HACK := $(shell mkdir -p $(OUT))

CSRCS = $(shell find . -name '*.c')
_COBJ =  $(notdir $(CSRCS))
COBJS = $(_COBJ:%.c=$(OUT)/%.o)

all: $(BINARY)

# This should be run as root because we'll modify the schedule policy
run: $(BINARY)
	sudo taskset -c 1 $(BINARY)
clean:
	$(RM) $(COBJS)
	$(RM) $(BINARY)
	$(RM) $(OUT)/*.d

$(BINARY): $(COBJS)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OUT)/%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

-include $(OUT)/*.d
