
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/reproduction/uterus/physiology.hpp>
#include <karkinolution/core/global_epsilon.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/utils/k_random.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>


NormalizedValue<float> UterusPhysiology::embryotrophy_energy(const Uterus& uterus, const Embryo& embryo) {
    float dist = embryo.hungry().ratio() - 0.85; // max = 0.15 (1 - 0.85)
    return RandomGenerators::generate(0.85 - dist, 1.0) / uterus.get_pregnant_uterus().embryos_count() + dist;
}
