/// \file lcg.hpp
/// \brief Модифицированный линейный конгруэнтный генератор
///
/// Базовый шаг: r_{n+1} = (a * r_n + c) mod 2^64
/// Параметры — каноничные значения из Numerical Recipes
/// (a = 6364136223846793005, c = 1442695040888963407, m = 2^64)
///
/// **Модификация**: после каждого шага результат пропускается через
/// XOR-shift старшие↔младшие биты. Это компенсирует известный
/// недостаток LCG — слабость младших разрядов (период 1 у младшего
/// бита для m = 2^k); похожая идея используется в PCG
#pragma once

#include <cstdint>

namespace pt::prng {

class LCG {
public:
    explicit LCG(std::uint64_t seed = 88172645463325252ULL) : state_(seed ? seed : 1) {}

    /// \brief Возвращает следующее 32-битное псевдослучайное число
    std::uint32_t next_u32() noexcept {
        state_ = state_ * A + C;
        // Тщательно смешиваем биты: старшие 32 ⊕ младшие 32 после поворота
        std::uint64_t mixed = state_ ^ (state_ >> 33);
        return static_cast<std::uint32_t>(mixed >> 32) ^ static_cast<std::uint32_t>(mixed);
    }

    /// \brief Возвращает число в [0,1)
    double next_double() noexcept {
        return next_u32() * (1.0 / 4294967296.0);
    }

    /// \brief Сброс генератора на новое семя
    void seed(std::uint64_t s) noexcept { state_ = s ? s : 1; }

private:
    static constexpr std::uint64_t A = 6364136223846793005ULL;
    static constexpr std::uint64_t C = 1442695040888963407ULL;
    std::uint64_t state_;
};

} // namespace pt::prng
