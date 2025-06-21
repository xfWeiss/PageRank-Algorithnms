CC = gcc
CFLAGS = -Wall -Wextra -Werror -march=native
LIB = -lm
STD = -std=c11 

SRC_GEN_DIR = src/Generator
SRC_PR_DIR = src/PageRank
BIN_DIR = bin

GEN_SATURATED = $(SRC_GEN_DIR)/gen-saturated.c
GEN_SPARSE = $(SRC_GEN_DIR)/gen-sparse.c
AT_PR = $(SRC_PR_DIR)/AT-PageRank.c
CRS_PR = $(SRC_PR_DIR)/CRS-PageRank.c

all: $(BIN_DIR)/AT-PageRank $(BIN_DIR)/CRS-PageRank

$(BIN_DIR)/AT-PageRank: $(AT_PR)
	$(CC) $(CFLAGS) $< -o $@ $(LIB) $(STD)

$(BIN_DIR)/CRS-PageRank: $(CRS_PR)
	$(CC) $(CFLAGS) $< -o $@ $(LIB) $(STD)

gen: $(BIN_DIR)/gen-saturated $(BIN_DIR)/gen-sparse

$(BIN_DIR)/gen-saturated: $(GEN_SATURATED)
	$(CC) $(CFLAGS) $< -o $@ $(STD)

$(BIN_DIR)/gen-sparse: $(GEN_SPARSE)
	$(CC) $(CFLAGS) $< -o $@ $(STD)

clean:
	rm -f bin/*

.PHONY: all gen clean