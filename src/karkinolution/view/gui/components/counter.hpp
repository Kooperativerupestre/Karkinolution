#pragma once
#include <raylib.h>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			class Counter {
				public:

					Counter(const int &initial_value, Vector2 position);

					void draw() const;

				private:

					const int &value_;
					Vector2    position_;
			};
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
