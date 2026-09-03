#pragma once
#include <karkinolution/organism/pathogens/virus/virus.hpp>

using GenericPathogenContainer = std::variant<VirusContainer>;

class PathogenRegistry : public BaseStorage<PathogenId, GenericPathogenContainer> {
	public:

		PathogenRegistry()
			: BaseStorage<PathogenId, GenericPathogenContainer>() {}

		VirusContainer &at_virus(PathogenId id) {
			return std::get<VirusContainer>(at(id));
		}

		const VirusContainer &at_virus(PathogenId id) const {
			return std::get<VirusContainer>(at(id));
		}
};