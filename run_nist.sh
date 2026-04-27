#!/usr/bin/env bash
# Запуск 5+ NIST-тестов через рефереферентную реализацию NIST STS.
#
# Перед использованием:
#   1. Скачать архив:
#        curl -L -o sts.zip https://csrc.nist.gov/CSRC/media/Projects/Random-Bit-Generation/documents/sts-2_1_2.zip
#      (или с GitHub-зеркала например terrillmoore/NIST-Statistical-Test-Suite)
#   2. unzip sts.zip -d nist_sts
#   3. cd nist_sts/sts-2.1.2 && make
#   4. Бинарь окажется в nist_sts/sts-2.1.2/assess
#
# Использование: ./run_nist.sh samples/lcg.bin 1000000
#                                     ^файл    ^кол-во бит на одну итерацию
set -euo pipefail

FILE="${1:-samples/lcg.bin}"
BITS="${2:-1000000}"
NIST_DIR="${NIST_DIR:-nist_sts/sts-2.1.2}"

if [ ! -x "$NIST_DIR/assess" ]; then
    echo "NIST STS не собран в $NIST_DIR. Смотри инструкции в шапке файла." >&2
    exit 1
fi

ITER=$((10))
echo "Прогон $FILE через NIST: $ITER итераций по $BITS бит"

cd "$NIST_DIR"
# В интерактивном режиме STS пройдёт меню; мы заскриптуем ввод.
# Меню: 0 ввод файла, путь, 1 включаемые тесты (mask), 1 запустить,
# выбираем 5 тестов: Frequency, BlockFrequency, Runs, LongestRun, CumulativeSums.
# Маска (15 тестов): 1 — включён, 0 — выключен; их порядок:
#  1 Frequency
#  2 BlockFrequency
#  3 CumulativeSums
#  4 Runs
#  5 LongestRun
#  6 Rank
#  7 FFT
#  8 NonOverlappingTemplate
#  9 OverlappingTemplate
# 10 Universal
# 11 ApproximateEntropy
# 12 RandomExcursions
# 13 RandomExcursionsVariant
# 14 Serial
# 15 LinearComplexity
MASK="111110000000000"

INPUT="../../$FILE"

./assess $BITS <<EOF
0
$INPUT
1
$MASK
1
$ITER
0
EOF

echo "Готово. Отчёт: $NIST_DIR/experiments/AlgorithmTesting/finalAnalysisReport.txt"
