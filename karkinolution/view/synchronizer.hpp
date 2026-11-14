#pragma once

#include "karkinolution/networking/client_side/client/client.hpp"
#include "karkinolution/view/screen.hpp"

namespace KarkinolutionView {
	class Synchronizer {
		private:

			Karkinolution::Networking::ClientSide::Client::RuntimeClient client;
			KarkinolutionView::Screen::State                             state{false};
			KarkinolutionView::Screen::StateView                         state_view{state};

		public:

			Synchronizer() = default;

			bool is_connected() const;
			bool connect();
			bool disconnect();

			void draw();
	};
} // namespace KarkinolutionView
