"""Графики ЛР3: распределение Хи-квадрат и сравнение скоростей."""
import csv
import sys
from pathlib import Path
from collections import defaultdict

import matplotlib.pyplot as plt

R = Path("results")


def load(path):
    with open(path, newline="") as f:
        return list(csv.DictReader(f))


def plot_chi():
    rows = load(R / "stats.csv")
    by_method = defaultdict(list)
    for r in rows:
        by_method[r["method"]].append(float(r["chi2"]))
    crit = float(rows[0]["chi_crit_005"])

    fig, ax = plt.subplots(figsize=(10, 6))
    xs = list(range(1, len(next(iter(by_method.values()))) + 1))
    for name, vals in by_method.items():
        ax.plot(xs, vals, marker="o", label=name)
    ax.axhline(crit, color="red", linestyle="--", label=f"χ²_крит={crit:.2f}")
    ax.set_xlabel("Номер выборки")
    ax.set_ylabel("χ² экспериментальный")
    ax.set_title("ЛР3: критерий Хи-квадрат для трёх ГПСЧ (α=0.05)")
    ax.grid(True, linestyle="--", alpha=0.4)
    ax.legend()
    fig.tight_layout()
    fig.savefig(R / "chi2.png", dpi=130)
    print("Сохранено:", R / "chi2.png")


def plot_speed():
    rows = [{k: float(v) for k, v in r.items()} for r in load(R / "bench.csv")]
    rows.sort(key=lambda r: r["n"])
    n = [r["n"] for r in rows]
    fig, ax = plt.subplots(figsize=(10, 6))
    for name, col in [("LCG", "lcg_ms"), ("LFG", "lfg_ms"),
                      ("MidSquare⊕", "midsq_ms"), ("std::mt19937", "mt19937_ms")]:
        ax.plot(n, [r[col] for r in rows], marker="o", label=name)
    ax.set_xscale("log"); ax.set_yscale("log")
    ax.set_xlabel("Объём выборки N"); ax.set_ylabel("Время, мс")
    ax.set_title("ЛР3: скорость генерации")
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    ax.legend()
    fig.tight_layout()
    fig.savefig(R / "speed.png", dpi=130)
    print("Сохранено:", R / "speed.png")


def main():
    if not (R / "stats.csv").exists() or not (R / "bench.csv").exists():
        sys.exit("Сначала: make stats bench")
    plot_chi()
    plot_speed()


if __name__ == "__main__":
    main()
