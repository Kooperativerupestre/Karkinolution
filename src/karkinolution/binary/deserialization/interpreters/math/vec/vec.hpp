#pragma once
#include "karkinolution/math/physic/vec/model.hpp"

#include <cstddef>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace DSI {
				namespace Vec {
					double get_x(const std::vector<std::byte> &payload, std::size_t offset);
					double get_y(const std::vector<std::byte> &payload, std::size_t offset);
					double get_z(const std::vector<std::byte> &payload, std::size_t offset);
					Vec3   get_vec(const std::vector<std::byte> &payload, std::size_t offset);
				} // namespace Vec
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
