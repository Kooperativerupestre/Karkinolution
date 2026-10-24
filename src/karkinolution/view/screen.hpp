#pragma once


#include "karkinolution/core/color.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <netinet/in.h>
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


			public:

				StateView(const State &state)
					: state(state) {}

				void draw() const;
		};
	} // namespace Screen
} // namespace KarkinolutionView