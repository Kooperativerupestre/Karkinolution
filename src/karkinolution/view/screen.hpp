#pragma once


#include "shared/core/color.hpp"
#include "karkinolution/view/gui/components/low_level/text.hpp"
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

				Gui::Components::LowLevel::Text connection_status{
					Gui::Geometry::Rectangle{Gui::Geometry::Position{110, 320},
											 Gui::Geometry::Width{140},
											 Gui::Geometry::Height{40}},
					"",
					KShared::Colors::Presets::DarkGray,
					20};

			public:

				StateView(const State &state)
					: state(state) {}

				void draw();
		};
	} // namespace Screen
} // namespace KarkinolutionView