#pragma once

#include "opsiamethystos/gui/components/component.hpp"
#include "opsiamethystos/gui/components/low_level/text.hpp"
#include "opsiamethystos/gui/panel/panel.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"

#include <memory>
#include <string>

namespace KarkinolutionView {
	namespace Gui = Opsiamethystos::Gui;

	namespace Screen {
		class State {
			public:

				bool connected = false;
		};

		class StateView {
			private:

				const State                                                &state_;
				std::shared_ptr<Opsiamethystos::Gui::Components::Component> component_;
				Gui::Components::LowLevel::Text                             fallback_status_{
                    Gui::Geometry::Rectangle{Gui::Geometry::Position{110, 320},
                                             Gui::Geometry::Width{140},
                                             Gui::Geometry::Height{40}},
                    "Disconnected",
                    KShared::Colors::Presets::DarkGray,
                    20};

			public:

				explicit StateView(const State &state);
				StateView(const State &state, Opsiamethystos::Gui::Panel::Panel &panel);

				void register_panel(Opsiamethystos::Gui::Panel::Panel &panel);
				void sync();
				void draw();
		};
	} // namespace Screen
} // namespace KarkinolutionView