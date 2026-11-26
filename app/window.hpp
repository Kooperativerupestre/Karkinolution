#pragma once
#include <app/presets/screen.hpp>

namespace KApp {
	namespace Window {

		bool should_window_close();
		void init_inside_loop();
		void init_before_loop(KApp::Presets::Screen screen_preset);

		void end_inside_loop();
		void end_after_loop();
	} // namespace Window
} // namespace KApp
