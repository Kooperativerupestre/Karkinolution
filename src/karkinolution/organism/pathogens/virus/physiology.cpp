#include <karkinolution/organism/pathogens/virus/physiology.hpp>
#include <karkinolution/organism/pathogens/virus/virus.hpp>



bool VirusPhysiology::is_dead(const GenericVirus&virus) {
    return VirusUtils::get_viral_load(virus.state).is_zero();
}

template <VirusType V>
[[nodiscard]] ViralLoad VirusPhysiology::calculate_min_viral_load(const V &virus,  const MetamorphosisEnvironment &environment) {
    return virus.viral_load.value() - environment.efficiency.value() * environment.immunity.value();
}
template <VirusType V>
[[nodiscard]] ViralLoad VirusPhysiology::calculate_max_viral_load(const V &virus, const MetamorphosisEnvironment &environment) {
    return virus.viral_load.value() + 0.5f - environment.efficiency.value() * environment.immunity.value();
}

[[nodiscard]] Vorax VirusPhysiology::metamorphosis(const Vorax&virus, const MetamorphosisEnvironment&environment) {
    const auto min_viral_load = calculate_min_viral_load(virus, environment);
    const auto max_viral_load = calculate_max_viral_load(virus, environment);

    const NormalizedValue<float> immunity_factor = 1.0f - (environment.immunity.value() - 0.25f);
    return Vorax{
        .id = VirusIDF::create_virus_id(VirusSpecies::VORAX),
        .viral_load = ViralLoad{RandomGenerators::generate(min_viral_load.value(), max_viral_load.value())},
        .consume_metabolism = virus.consume_metabolism.value() * immunity_factor.value(),
        .consume_muscles = virus.consume_muscles.value() * immunity_factor.value()
    };
}

[[nodiscard]] MorbusExiguus VirusPhysiology::metamorphosis(const MorbusExiguus &virus, const MetamorphosisEnvironment &environment) {
    const auto min_viral_load = calculate_min_viral_load(virus, environment);
    const auto max_viral_load = calculate_max_viral_load(virus, environment);

    return MorbusExiguus{
        .id = VirusIDF::create_virus_id(VirusSpecies::MORBUS_EXIGUUS),
        .viral_load = ViralLoad{RandomGenerators::generate(min_viral_load.value(),  max_viral_load.value())},
        .efficiency = virus.efficiency.value() * environment.immunity_factor().value()
    };
}