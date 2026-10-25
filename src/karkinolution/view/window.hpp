#pragma once
#include <karkinolution/view/presets/screen.hpp>

namespace KarkinolutionView {
	namespace Window {

		void init_inside_loop();
		void init_before_loop(Presets::Screen screen_preset);

		void end_inside_loop();
		void end_after_loop();
	} // namespace Window
} // namespace KarkinolutionView