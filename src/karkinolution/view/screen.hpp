#pragma once


#include "karkinolution/core/color.hpp"
#include "karkinolution/view/gui/components/text.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <string>

namespace KarkinolutionView {

	namespace Screen {
		class State {
			public:

				bool connected = false;
		};

		class StateView {
			private:

				const State &state;

				Gui::Components::Text connection_status = Gui::Components::Text{
					.text      = "",
					.color     = Karkinolution::Colors::Presets::DarkGray,
					.position  = KarkinolutionView::Gui::Geometry::Position{100, 300},
					.font_size = 20};


			public:

				StateView(const State &state)
					: state(state) {}

				void draw();
		};
	} // namespace Screen
} // namespace KarkinolutionView