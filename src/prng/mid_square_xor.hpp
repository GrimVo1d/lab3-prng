/// \file mid_square_xor.hpp
/// \brief Метод середины квадратов с XOR-перемешиванием
///
/// Классический метод Дж. фон Неймана (1949) — возведение состояния в
/// квадрат и извлечение средних бит; известная слабость — вырождение
/// в 0 или малый цикл
///
/// **Модификация**: добавлено XOR-перемешивание перед возведением в
/// квадрат (циклический сдвиг + XOR с маской-константой); это
/// гарантированно не даёт состоянию занулиться и существенно
/// удлиняет период по сравнению с классическим вариантом
#pragma once

#include <cstdint>

namespace pt::prng {

class MidSquareXor {
public:
    explicit MidSquareXor(std::uint64_t seed = 0x1234567812345678ULL)
        : state_(seed ? seed : 1) {}

    std::uint32_t next_u32() noexcept {
        // 1) перемешивание: циклический сдвиг + XOR с константой
        std::uint64_t s = state_;
        s = ((s << 17) | (s >> 47)) ^ MASK;

        // 2) возведение в квадрат (берём младшие 64 бита от 128-битного произведения)
        __uint128_t sq = static_cast<__uint128_t>(s) * static_cast<__uint128_t>(s);
        std::uint64_t low = static_cast<std::uint64_t>(sq);
        std::uint64_t high = static_cast<std::uint64_t>(sq >> 64);
        std::uint64_t mid = (low >> 16) ^ (high << 48); // средние биты

        state_ = mid | 1ULL; // не даём занулиться
        return static_cast<std::uint32_t>(mid >> 16); // средние 32 бита
    }

    double next_double() noexcept {
        return next_u32() * (1.0 / 4294967296.0);
    }

    void seed(std::uint64_t s) noexcept { state_ = s ? s : 1; }

private:
    static constexpr std::uint64_t MASK = 0xA5A5A5A5A5A5A5A5ULL;
    std::uint64_t state_;
};

} // namespace pt::prng
