#include "karkinolution/math/stats/compile_values.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/pathogens/virus/virus.hpp>

VirusId VirusUtils::get_id(const AllVirusType &virus) {
	return std::visit(
		[](const auto &virus) {
			return virus.id;
		},
		virus);
}

VirusId GenericVirus::id() const {
	return VirusUtils::get_id(state);
}

[[nodiscard]] NormalizedValue<float> MetamorphosisEnvironment::immunity_factor() const noexcept {
	return 1.0f - (immunity.value() - 0.25f);
}
