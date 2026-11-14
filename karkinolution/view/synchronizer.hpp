#pragma once

#include "karkinolution/networking/client_side/client/client.hpp"
#include "karkinolution/view/client/client.hpp"
#include "karkinolution/view/screen.hpp"
#include "karkinolution/view/world/world.hpp"

#include <optional>

namespace KarkinolutionView {
	class Synchronizer {
		private:

			KarkinolutionView::Client::Client              client;
			std::optional<KarkinolutionView::World::World> world;
			KarkinolutionView::Screen::State               state{false};
			KarkinolutionView::Screen::StateView           state_view{state};

		public:

			Synchronizer() = default;

			bool is_connected() const;
			bool connect();
			bool disconnect();

			void draw();

			Karkinolution::Networking::ClientSide::Client::ResponseResult
			request_and_sync(const Karkinolution::Networking::ClientSide::Client::RequestData &data);
	};
} // namespace KarkinolutionView
