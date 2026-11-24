#pragma once

#include "karkinolution/networking/frame/base/contract.hpp"
#include "shared/binary/fields.hpp"

#include <cstddef>

namespace Karkinolution::Networking::Frames::Base::Layout {

	using DeclaredSize = KShared::Binary::Field<0, sizeof(std::uint32_t)>;
	using Type         = KShared::Binary::Next<DeclaredSize, sizeof(std::uint8_t)>;
	using SubType      = KShared::Binary::Next<Type, sizeof(std::uint8_t)>;

	inline constexpr std::size_t HEADER_SIZE           = SubType::END;
	inline constexpr std::size_t MINIMUM_DECLARED_SIZE = HEADER_SIZE - DeclaredSize::SIZE;

} // namespace Karkinolution::Networking::Frames::Base::Layout
