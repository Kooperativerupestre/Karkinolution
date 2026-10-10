#pragma once

#include <cstddef>
#include <karkinolution/math/units.hpp>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace DSI {

				namespace Territory {
					namespace Models {
						class Territory {
							public:

								Size size;
						};
					} // namespace Models

					Models::Territory deserialize_territory(const std::vector<std::byte> &payload);
				} // namespace Territory
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
