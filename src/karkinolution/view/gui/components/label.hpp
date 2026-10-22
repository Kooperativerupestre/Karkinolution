#pragma once
#include <raygui.h>
#include <string_view>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {


			class Label {
				public:

					Label(std::string_view text, Rectangle bounds);

					void draw() const;

				private:

					std::string_view text_;
					Rectangle        bounds_;
			};
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView