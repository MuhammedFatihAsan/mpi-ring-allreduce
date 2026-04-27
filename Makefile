# Derleyici ve Bayraklar
CC = mpicc
CFLAGS = -Iinclude -Wall -O3 -pg
LDFLAGS = -lm

# Klasör Yapısı
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Kaynak Dosyalar
COMMON_SRC = $(SRC_DIR)/common.c
BASELINE_SRC = $(SRC_DIR)/baseline.c
RING_SRC = $(SRC_DIR)/ring_allreduce.c

# Nesne Dosyaları
COMMON_OBJ = $(OBJ_DIR)/common.o
BASELINE_OBJ = $(OBJ_DIR)/baseline.o
RING_OBJ = $(OBJ_DIR)/ring_allreduce.o

# --- HEDEFLER (TARGETS) ---

# 1. make all: Her iki yöntemi de derler
all: directories $(BIN_DIR)/baseline $(BIN_DIR)/ring

# 2. make baseline: Sadece standart yöntemi derler
baseline: directories $(BIN_DIR)/baseline

# 3. make ring: Sadece Ring yöntemini derler
ring: directories $(BIN_DIR)/ring

# Klasörleri oluşturma
directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

# Baseline ikili dosyası
$(BIN_DIR)/baseline: $(BASELINE_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Ring ikili dosyası
$(BIN_DIR)/ring: $(RING_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Nesne dosyaları (.o) oluşturma kuralı
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 4. make clean: Derlenmiş dosyaları ve profiling verilerini temizler
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) gmon.out

.PHONY: all baseline ring clean directories