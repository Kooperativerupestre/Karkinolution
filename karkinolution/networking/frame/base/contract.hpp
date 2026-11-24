#pragma once

#include "karkinolution/networking/frame/system/contract.hpp"
#include "karkinolution/networking/frame/world/contract.hpp"

#include <cstdint>
#include <variant>

namespace Karkinolution::Networking::Frames::Base {

	// The base variants preserve the frame scope because World and System reuse
	// numeric type values while defining different protocol meanings.
	using TypeVariant    = std::variant<World::Type, System::Type>;
	using SubTypeVariant = std::variant<World::Detail::SubTypeVariant, System::Detail::SubTypeVariant>;

	enum class FrameExistence : std::uint8_t {
		YES,
		INCOMPLETE,
		DECLARED_SIZE_COMPLETE,
		DECLARED_SIZE_INCOMPLETE,
		NO
	};

} // namespace Karkinolution::Networking::Frames::Base
