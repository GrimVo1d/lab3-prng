/// \file main.cpp
/// \brief Точка входа ЛР3: генерация выборок, статистика, бенчмарки
///
/// Режимы:
///   ./hw3 stats   STATS.csv         — 20 выборок × 3 метода × 1000 чисел,
///                                     считает mean/std/CV/χ² для каждой
///   ./hw3 bench   BENCH.csv         — замеры скорости 1k..1M (наши 3 + mt19937)
///   ./hw3 sample  METHOD N OUT.bin  — пишет бинарную выборку (для NIST STS)
///                  METHOD = lcg | lfg | midsq

#include "prng/lcg.hpp"
#include "prng/lfg.hpp"
#include "prng/mid_square_xor.hpp"
#include "stats.hpp"
#include "timing.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

template <typename Gen>
std::vector<double> sample_doubles(Gen& g, std::size_t n) {
    std::vector<double> out(n);
    for (std::size_t i = 0; i < n; ++i) out[i] = g.next_double();
    return out;
}

template <typename Gen>
void write_binary(Gen& g, std::size_t n_uint32, const std::string& path) {
    std::ofstream f(path, std::ios::binary);
    for (std::size_t i = 0; i < n_uint32; ++i) {
        std::uint32_t v = g.next_u32();
        f.write(reinterpret_cast<const char*>(&v), sizeof(v));
    }
}

int run_stats(const std::string& out_path) {
    std::ofstream out(out_path);
    if (!out) { std::cerr << "Не открыть " << out_path << "\n"; return 1; }
    out << "method,sample_idx,mean,std,cv,chi2,df,chi_crit_005,uniform_ok\n";

    constexpr int N_SAMPLES = 20;
    constexpr std::size_t SIZE = 5000; // не менее 5000 чисел в выборке
    const int K = pt::stats::sturges_k(SIZE);
    const double crit = pt::stats::chi_critical_005(K - 1);

    auto run = [&](const std::string& name, auto factory) {
        for (int i = 0; i < N_SAMPLES; ++i) {
            auto g = factory(static_cast<std::uint64_t>(i) * 0x9E3779B97F4A7C15ULL + 1);
            auto data = sample_doubles(g, SIZE);
            auto s = pt::stats::summarize(data);
            auto [chi2, df] = pt::stats::chi_square_uniform(data, K);
            bool ok = chi2 <= crit;
            out << name << ',' << i << ','
                << s.mean << ',' << s.std_dev << ',' << s.cv << ','
                << chi2 << ',' << df << ',' << crit << ','
                << (ok ? 1 : 0) << '\n';
            std::cerr << name << "[" << i << "] mean=" << s.mean
                      << " std=" << s.std_dev
                      << " CV=" << s.cv
                      << " chi2=" << chi2 << "/" << crit
                      << (ok ? " OK" : " FAIL") << '\n';
        }
    };

    run("lcg",   [](std::uint64_t s){ return pt::prng::LCG(s); });
    run("lfg",   [](std::uint64_t s){ return pt::prng::LFG(s); });
    run("midsq", [](std::uint64_t s){ return pt::prng::MidSquareXor(s); });
    return 0;
}

int run_bench(const std::string& out_path) {
    std::ofstream out(out_path);
    if (!out) return 1;
    out << "n,lcg_ms,lfg_ms,midsq_ms,mt19937_ms\n";
    for (std::size_t n : {1000ULL, 10000ULL, 100000ULL, 500000ULL, 1000000ULL}) {
        pt::prng::LCG  a(1);
        pt::prng::LFG  b(2);
        pt::prng::MidSquareXor c(3);
        std::mt19937 d(4);

        double t_lcg = pt::measure_ms([&] {
            volatile std::uint32_t x = 0;
            for (std::size_t i = 0; i < n; ++i) x ^= a.next_u32();
            (void)x;
        });
        double t_lfg = pt::measure_ms([&] {
            volatile std::uint32_t x = 0;
            for (std::size_t i = 0; i < n; ++i) x ^= b.next_u32();
            (void)x;
        });
        double t_ms = pt::measure_ms([&] {
            volatile std::uint32_t x = 0;
            for (std::size_t i = 0; i < n; ++i) x ^= c.next_u32();
            (void)x;
        });
        double t_mt = pt::measure_ms([&] {
            volatile std::uint32_t x = 0;
            for (std::size_t i = 0; i < n; ++i) x ^= d();
            (void)x;
        });
        out << n << ',' << t_lcg << ',' << t_lfg << ','
            << t_ms << ',' << t_mt << '\n';
        std::cerr << "n=" << n
                  << "  lcg=" << t_lcg
                  << "  lfg=" << t_lfg
                  << "  midsq=" << t_ms
                  << "  mt19937=" << t_mt << " ms\n";
    }
    return 0;
}

int run_sample(const std::string& method, std::size_t n, const std::string& out_path) {
    if (method == "lcg") {
        pt::prng::LCG g(0x1234ULL);
        write_binary(g, n, out_path);
    } else if (method == "lfg") {
        pt::prng::LFG g(0x5678ULL);
        write_binary(g, n, out_path);
    } else if (method == "midsq") {
        pt::prng::MidSquareXor g(0x9ABCULL);
        write_binary(g, n, out_path);
    } else {
        std::cerr << "Неизвестный метод: " << method << "\n";
        return 1;
    }
    std::cerr << "Записано " << n << " uint32 в " << out_path << "\n";
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    try {
        if (argc >= 3 && std::string(argv[1]) == "stats")
            return run_stats(argv[2]);
        if (argc >= 3 && std::string(argv[1]) == "bench")
            return run_bench(argv[2]);
        if (argc >= 5 && std::string(argv[1]) == "sample")
            return run_sample(argv[2], std::stoull(argv[3]), argv[4]);
        std::cerr << "Использование:\n"
                  << "  " << argv[0] << " stats  STATS.csv\n"
                  << "  " << argv[0] << " bench  BENCH.csv\n"
                  << "  " << argv[0] << " sample lcg|lfg|midsq N OUT.bin\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 2;
    }
}
