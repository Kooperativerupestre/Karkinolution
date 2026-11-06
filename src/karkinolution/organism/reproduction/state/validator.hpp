#pragma once
#include "karkinolution/organism/reproduction/state/ontology.hpp"

#include <cassert>
#include <karkinolution/organism/reproduction/state/state.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <variant>

namespace Karkinolution::Organisms::Entities::Reproduction::State::Validator {

	inline void has_uterus(const Karkinolution::Organisms::Entities::Reproduction::ReproductiveState &reproduction) {
		assert(std::holds_alternative<Karkinolution::Organisms::Entities::Reproduction::Uterus::Uterus>(
			reproduction.state));
	}

	inline void
	does_not_have_uterus(const Karkinolution::Organisms::Entities::Reproduction::ReproductiveState &reproduction) {
		assert(std::holds_alternative<std::monostate>(reproduction.state));
	}

	inline void is_pregnant(const Karkinolution::Organisms::Entities::Reproduction::ReproductiveState &reproduction) {
		has_uterus(reproduction);

		assert(std::get<Karkinolution::Organisms::Entities::Reproduction::Uterus::Uterus>(reproduction.state)
				   .is_pregnant());
	}

	inline void
	is_not_pregnant(const Karkinolution::Organisms::Entities::Reproduction::ReproductiveState &reproduction) {
		has_uterus(reproduction);
		assert(!std::get<Karkinolution::Organisms::Entities::Reproduction::Uterus::Uterus>(reproduction.state)
					.is_pregnant());
	}

	inline void is_oviparous(const Karkinolution::Organisms::Entities::Reproduction::ReproductiveState &reproduction) {
		assert(reproduction.reproductive_way == Ontology::ReproductiveWays::OVIPAROUS);
	}

	inline void is_viviparous(const Karkinolution::Organisms::Entities::Reproduction::ReproductiveState &reproduction) {
		assert(reproduction.reproductive_way == Ontology::ReproductiveWays::VIVIPAROUS);
	}

} // namespace Karkinolution::Organisms::Entities::Reproduction::State::Validator
