/// \file lfg.hpp
/// \brief Lagged Fibonacci Generator (Фибоначчи с запаздыванием), XOR-вариант
///
/// Формула:
///   x_k = x_{k-p} XOR x_{k-p+q}
/// Параметры выбраны как классические Mitchell-Moore: p = 55, q = 24
/// Период при использовании XOR — 2^55 − 1 (это связано с фактом, что
/// многочлен x^55 + x^24 + 1 примитивен над GF(2))
///
/// Инициализация состояния — отдельным LCG, чтобы не получить нулевое
/// начальное состояние (тогда LFG залип бы в нуле)
#pragma once

#include "lcg.hpp"

#include <array>
#include <cstdint>

namespace pt::prng {

class LFG {
public:
    explicit LFG(std::uint64_t seed = 0xDEADBEEFCAFEBABEULL) { this->seed(seed); }

    std::uint32_t next_u32() noexcept {
        std::uint32_t v = state_[idx_p_] ^ state_[idx_q_];
        state_[idx_p_] = v;
        idx_p_ = (idx_p_ + 1) % P;
        idx_q_ = (idx_q_ + 1) % P;
        return v;
    }

    double next_double() noexcept {
        return next_u32() * (1.0 / 4294967296.0);
    }

    void seed(std::uint64_t s) noexcept {
        LCG init(s);
        for (std::size_t i = 0; i < P; ++i) state_[i] = init.next_u32();
        // Гарантируем что не все нули в младших битах (требование LFG)
        state_[0] |= 1u;
        idx_p_ = 0;
        idx_q_ = P - Q;
    }

private:
    static constexpr std::size_t P = 55;
    static constexpr std::size_t Q = 24;
    std::array<std::uint32_t, P> state_{};
    std::size_t idx_p_ = 0;
    std::size_t idx_q_ = 0;
};

} // namespace pt::prng
