#pragma once
#include <karkinolution/binary/interpreters/models/soil.hpp>
#include <karkinolution/core/id.hpp>
#include <karkinolution/view/client/client.hpp>

namespace KarkinolutionView {
	namespace World {
		namespace Queriers {
			namespace Soil {
				std::optional<Karkinolution::Binary::Interpreters::Models::Soil::Soil>
				see_soil(BaseIdType id, KarkinolutionView::Client::Client &client);
			} // namespace Soil
		} // namespace Queriers
	} // namespace World
} // namespace KarkinolutionView