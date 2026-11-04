#pragma once

#include "karkinolution/view/gui/components/identity.hpp"

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			template <ComponentType T> class BaseComponent {
				private:

					ComponentId id_ = gen_id(T);

				public:

					const ComponentId &get_id() const noexcept {
						return id_;
					}

					const ComponentId &id() const noexcept {
						return get_id();
					}
			};
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView