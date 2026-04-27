/// \file timing.hpp
/// \brief Замер времени через std::chrono::steady_clock
#pragma once
#include <chrono>
namespace pt {
template <typename F>
double measure_ms(F&& f) {
    auto t0 = std::chrono::steady_clock::now();
    f();
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
}
} // namespace pt
