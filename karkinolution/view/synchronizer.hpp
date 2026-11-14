#pragma once

#include "karkinolution/view/client/client.hpp"
#include "karkinolution/view/screen.hpp"

namespace KarkinolutionView {
	class Synchronizer {
		private:

			KarkinolutionView::Client::Client    client;
			KarkinolutionView::Screen::State     state{false};
			KarkinolutionView::Screen::StateView state_view{state};

		public:

			Synchronizer() = default;

			bool is_connected() const;
			bool connect();
			bool disconnect();

			void draw();
	};
} // namespace KarkinolutionView
