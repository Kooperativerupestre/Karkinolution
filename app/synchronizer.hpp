#pragma once

#include "app/creature_creator.hpp"
#include "app/screen.hpp"
#include "karkinolution/networking/client_side/client/client.hpp"
#include "opsiamethystos/graphics/panel/panel.hpp"

namespace KarkinolutionView {
	class Synchronizer {
		private:

			Karkinolution::Networking::ClientSide::Client::RuntimeClient client;
			CreatureCreator                                              creature_creator;
			KarkinolutionView::Screen::State                             state{false};
			KarkinolutionView::Screen::StateView                         state_view;

		public:

			Synchronizer()
				: creature_creator(client)
				, state_view(state) {}

			explicit Synchronizer(Opsiamethystos::Gui::Panel::Panel &panel)
				: creature_creator(client)
				, state_view(state, panel) {}

			void register_panel(Opsiamethystos::Gui::Panel::Panel &panel) {
				state_view.register_panel(panel);
			}

			bool is_connected() const;
			bool connect();
			bool disconnect();
			void create_creature();
			void poll();

			[[nodiscard]] const auto &creatures() const {
				return creature_creator.creatures();
			}

			[[nodiscard]] const auto &territory_size() const {
				return creature_creator.territory_size();
			}

			void draw();
	};
} // namespace KarkinolutionView
