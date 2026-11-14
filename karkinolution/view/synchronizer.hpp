#pragma once

#include "karkinolution/networking/client_side/client/client.hpp"
#include "karkinolution/view/screen.hpp"
#include "opsiamethystos/gui/panel/panel.hpp"

namespace KarkinolutionView {
	class Synchronizer {
		private:

			Karkinolution::Networking::ClientSide::Client::RuntimeClient client;
			KarkinolutionView::Screen::State                             state{false};
			KarkinolutionView::Screen::StateView                         state_view;

		public:

			Synchronizer()
				: state_view(state) {}

			explicit Synchronizer(Opsiamethystos::Gui::Panel::Panel &panel)
				: state_view(state, panel) {}

			void register_panel(Opsiamethystos::Gui::Panel::Panel &panel) {
				state_view.register_panel(panel);
			}

			bool is_connected() const;
			bool connect();
			bool disconnect();

			void draw();
	};
} // namespace KarkinolutionView
