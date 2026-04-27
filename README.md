# ЛР3 — Генераторы псевдослучайных чисел

Лабораторная работа №3 по дисциплине «Программные технологии».

**Реализованы три собственных (модифицированных) ГПСЧ:**

1. **LCG** — линейный конгруэнтный генератор с XOR-shift доработкой
   младших разрядов (идея PCG).
2. **LFG** — Lagged Fibonacci Generator (p=55, q=24) с XOR.
3. **MidSquareXor** — метод середины квадратов фон Неймана с
   XOR-перемешиванием и защитой от вырождения.

В качестве эталона использован `std::mt19937` из `<random>`.


## Сборка и прогон

```bash
make stats bench samples plots docs
```

Цели Makefile:

- `make` — собрать `hw3`;
- `make stats` — 20 выборок × 3 генератора + критерий χ² → `results/stats.csv`;
- `make bench` — замер скорости → `results/bench.csv`;
- `make samples` — бинарные выборки (250 000 × uint32 = 8 млн бит) для NIST;
- `make plots` — графики (`results/chi2.png`, `results/speed.png`);
- `make docs` — Doxygen HTML.

## Тесты NIST

```bash
make samples
./run_nist.sh samples/lcg.bin
./run_nist.sh samples/lfg.bin
./run_nist.sh samples/midsq.bin
```

Скрипт требует, чтобы рядом находилась сборка
[NIST Statistical Test Suite](https://csrc.nist.gov/projects/random-bit-generation/documentation-and-software).

## Зависимости

- C++17 (`clang++` или `g++`)
- GNU Make
- Python 3 + `matplotlib` + `pandas` (для `make plots`)
- Doxygen (для `make docs`)
- NIST STS (опционально — для `run_nist.sh`)

## Структура

```
src/
  prng/
    lcg.hpp
    lfg.hpp
    mid_square_xor.hpp
  stats.hpp
  timing.hpp
  main.cpp
samples/          бинарные выборки для NIST
results/          stats.csv, bench.csv, *.png
docs/html/        Doxygen
Makefile
Doxyfile
plot.py
run_nist.sh
```
