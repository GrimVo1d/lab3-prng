CXX      ?= clang++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
SRC_DIR  := src
OBJ_DIR  := build

HW3_OBJS := $(OBJ_DIR)/main.o

.PHONY: all clean docs stats bench samples plots nist

all: hw3

hw3: $(HW3_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

stats: hw3
	mkdir -p results
	./hw3 stats results/stats.csv

bench: hw3
	mkdir -p results
	./hw3 bench results/bench.csv

# Бинарные выборки для NIST STS: 250000 uint32 = 8 миллионов бит на метод.
samples: hw3
	mkdir -p samples
	./hw3 sample lcg   250000 samples/lcg.bin
	./hw3 sample lfg   250000 samples/lfg.bin
	./hw3 sample midsq 250000 samples/midsq.bin

plots: stats bench
	python3 plot.py

docs:
	doxygen Doxyfile

clean:
	rm -rf $(OBJ_DIR) hw3 results/*.csv results/*.png samples/*.bin docs/html
