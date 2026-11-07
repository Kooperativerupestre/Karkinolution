#pragma once
#include <karkinolution/binary/interpreters/models/soil.hpp>
#include <karkinolution/view/client/client.hpp>
#include <shared/core/id.hpp>

namespace KarkinolutionView {
	namespace World {
		namespace Queriers {
			namespace Soil {
				std::optional<Karkinolution::Binary::Interpreters::Models::Soil::Soil>
				see_soil(KShared::Core::Id::BaseIdType id, KarkinolutionView::Client::Client &client);
				std::vector<Karkinolution::Binary::Interpreters::Models::Soil::Soil>
				see_all_soils(KarkinolutionView::Client::Client &client);
			} // namespace Soil
		} // namespace Queriers
	} // namespace World
} // namespace KarkinolutionView