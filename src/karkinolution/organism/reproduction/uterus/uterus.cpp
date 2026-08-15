#include <karkinolution/math/stats/runtime_values.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>


// Gestation


// Embryo



// PregnantUterus

int PregnantUterus::embryos_count() const {
    return embryos.size();
}

int PregnantUterus::dead_embryos_count() const {
    return born_count.max() - embryos_count() - embryos_count();
}

bool PregnantUterus::all_children_borned() const {
    return embryos_count() == 0;
}

bool PregnantUterus::has_embryos() const {
    return embryos_count() > 0;
}

bool PregnantUterus::is_full() const {
    return embryos_count() == born_count.max();
}

// Uterus

NormalizedValue<float> Uterus::hungry() const {
    return 1 -  energy.ratio();
}

float Uterus::pregnancy_cost() const {
    return 0;
}

bool Uterus::is_pregnant() const {
    return std::holds_alternative<PregnantUterus>(state);
}
bool Uterus::is_empty() const {
    return std::holds_alternative<EmptyUterus>(state);
}


PregnantUterus& Uterus::get_pregnant_uterus() {
    return std::get<PregnantUterus>(state);
}

const PregnantUterus& Uterus::get_pregnant_uterus() const {
    return std::get<PregnantUterus>(state);
}