/// \file stats.hpp
/// \brief Статистические функции и критерий Хи-квадрат
#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

namespace pt::stats {

struct Summary {
    double mean;
    double variance;   ///< дисперсия (по N, biased)
    double std_dev;
    double cv;         ///< коэффициент вариации = std / mean
};

inline Summary summarize(const std::vector<double>& x) {
    Summary s{};
    if (x.empty()) return s;
    double sum = 0.0;
    for (double v : x) sum += v;
    s.mean = sum / x.size();
    double q = 0.0;
    for (double v : x) q += (v - s.mean) * (v - s.mean);
    s.variance = q / x.size();
    s.std_dev = std::sqrt(s.variance);
    s.cv = (s.mean != 0.0) ? (s.std_dev / s.mean) : 0.0;
    return s;
}

/// \brief Критерий Хи-квадрат для проверки равномерности на [0,1)
/// \param x        выборка (значения в [0,1))
/// \param k        число интервалов
/// \return пара {chi2_observed, degrees_of_freedom}
inline std::pair<double, int> chi_square_uniform(
        const std::vector<double>& x, int k) {
    std::vector<long long> n_i(k, 0);
    for (double v : x) {
        int idx = std::min(static_cast<int>(v * k), k - 1);
        if (idx < 0) idx = 0;
        ++n_i[idx];
    }
    double expected = static_cast<double>(x.size()) / k;
    double chi2 = 0.0;
    for (int i = 0; i < k; ++i) {
        double d = n_i[i] - expected;
        chi2 += d * d / expected;
    }
    return {chi2, k - 1};
}

/// \brief Число интервалов по правилу Стерджеса: k = 1 + ⌈log2 N⌉
inline int sturges_k(std::size_t n) {
    int k = 1 + static_cast<int>(std::ceil(std::log2(static_cast<double>(n))));
    return std::max(k, 5);
}

/// \brief Критическое значение χ² для α = 0.05 (таблица)
/// Аппроксимация Wilson–Hilferty для df > 30:
///   χ²_крит ≈ df · (1 − 2/(9·df) + z_α · √(2/(9·df)))^3
/// где z_{0.05} = 1.6449; для df ≤ 30 — табличные значения
inline double chi_critical_005(int df) {
    static const double table[] = {
        0.0,    // df=0
        3.841, 5.991, 7.815, 9.488, 11.070, 12.592, 14.067, 15.507, 16.919, 18.307,
        19.675, 21.026, 22.362, 23.685, 24.996, 26.296, 27.587, 28.869, 30.144, 31.410,
        32.671, 33.924, 35.172, 36.415, 37.652, 38.885, 40.113, 41.337, 42.557, 43.773
    };
    if (df >= 1 && df <= 30) return table[df];
    if (df < 1) return 0.0;
    constexpr double z = 1.6449;
    double a = 2.0 / (9.0 * df);
    double t = 1.0 - a + z * std::sqrt(a);
    return df * t * t * t;
}

} // namespace pt::stats
