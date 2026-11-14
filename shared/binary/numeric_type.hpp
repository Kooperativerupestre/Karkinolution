#pragma once

#include <cstdint>

namespace KShared::Binary {

	enum class NumericType : std::uint8_t {
		FLOAT  = 1,
		DOUBLE = 2,
		UINT8  = 3,
		UINT16 = 4,
		UINT32 = 5,
		UINT64 = 6,
		INT    = 7
	};

} // namespace KShared::Binary
