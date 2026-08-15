#pragma once
#include <cstdint>
#include <karkinolution/math/stats/runtime_values.hpp>

class Gestation : public IntegerWithMax<uint16_t> {
    public:

    using IntegerWithMax::IntegerWithMax;
    float progress() const;
    bool is_ready_to_born() const;
    uint16_t overdue() const;
};