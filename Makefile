CC = mpicc
# -pg is off by default — mixing it with -O3 inlines everything and gprof shows nothing.
# use `make profile` when you actually want a gprof-able binary.
CFLAGS ?= -Iinclude -Wall -O3
LDFLAGS = -lm

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

COMMON_SRC = $(SRC_DIR)/common.c
BASELINE_SRC = $(SRC_DIR)/baseline.c
RING_SRC = $(SRC_DIR)/ring_allreduce.c

COMMON_OBJ = $(OBJ_DIR)/common.o
BASELINE_OBJ = $(OBJ_DIR)/baseline.o
RING_OBJ = $(OBJ_DIR)/ring_allreduce.o

all: directories $(BIN_DIR)/baseline $(BIN_DIR)/ring

baseline: directories $(BIN_DIR)/baseline

ring: directories $(BIN_DIR)/ring

# gprof build: -O0 so functions don't get inlined away
profile:
	$(MAKE) clean
	$(MAKE) all CFLAGS="-Iinclude -Wall -O0 -pg"

directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

$(BIN_DIR)/baseline: $(BASELINE_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR)/ring: $(RING_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) gmon.out

.PHONY: all baseline ring profile clean directories
