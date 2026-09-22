#pragma once
#include "karkinolution/math/physic/vec/model.hpp"

#include <cstddef>
#include <vector>

namespace VecDSI {
	Vec3 deserialize_vec(const std::vector<std::byte> &payload, std::size_t offset);
} // namespace VecDSI