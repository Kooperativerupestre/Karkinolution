#pragma once

#include <karkinolution/binary/interpreters/models/territory.hpp>
#include <karkinolution/view/client/client.hpp>

namespace KarkinolutionView {
	namespace World {
		namespace Queriers {
			namespace Territory {
				std::optional<Karkinolution::Binary::Interpreters::Models::Territory::Territory>
				see_territory(KarkinolutionView::Client::Client &client);
			} // namespace Territory
		} // namespace Queriers
	} // namespace World
} // namespace KarkinolutionView
