#pragma once

#include "karkinolution/networking/frame/base/layout.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/errors.hpp"

#include <format>
#include <type_traits>

namespace Karkinolution::Networking::Frames::Base::Parser {

	template <KShared::Binary::ByteRange T> std::uint32_t get_declared_size(const T &bytes) {
		return KShared::Binary::Deserializer::read_uint32_t(bytes, Layout::DeclaredSize::OFFSET);
	}

	template <typename ScopeType, KShared::Binary::ByteRange T> TypeVariant get_type(const T &bytes) {
		static_assert(std::is_same_v<ScopeType, World::Type> || std::is_same_v<ScopeType, System::Type>);
		return ScopeType{KShared::Binary::Deserializer::read_uint8_t(bytes, Layout::Type::OFFSET)};
	}

	template <typename ScopeType, KShared::Binary::ByteRange T> SubTypeVariant get_sub_type(const T &bytes) {
		static_assert(std::is_same_v<ScopeType, World::Type> || std::is_same_v<ScopeType, System::Type>);
		const auto raw = KShared::Binary::Deserializer::read_uint8_t(bytes, Layout::SubType::OFFSET);

		if constexpr (std::is_same_v<ScopeType, World::Type>) {
			const auto type = std::get<World::Type>(get_type<ScopeType>(bytes));
			if (type == World::Type::Request) {
				return World::Detail::SubTypeVariant{World::Request::SubType{raw}};
			}
			if (type == World::Type::Error) {
				return World::Detail::SubTypeVariant{World::Error::SubType{raw}};
			}
			return World::Detail::SubTypeVariant{World::Response::SubType{raw}};
		} else {
			const auto type = std::get<System::Type>(get_type<ScopeType>(bytes));
			if (type == System::Type::SystemCall) {
				return System::Detail::SubTypeVariant{System::SystemCall::SubType{raw}};
			}
			return System::Detail::SubTypeVariant{System::SystemOutput::SubType{raw}};
		}
	}

	template <KShared::Binary::ByteRange T> bool has_declared_size(const T &bytes) {
		return bytes.size() >= Layout::DeclaredSize::END;
	}

	template <KShared::Binary::ByteRange T> bool has_type(const T &bytes) {
		return bytes.size() >= Layout::Type::END;
	}

	template <KShared::Binary::ByteRange T> bool has_sub_type(const T &bytes) {
		return bytes.size() >= Layout::SubType::END;
	}

	template <KShared::Binary::ByteRange T> FrameExistence has_header(const T &bytes) {
		if (bytes.empty()) {
			return FrameExistence::NO;
		}
		if (bytes.size() < Layout::DeclaredSize::END) {
			return FrameExistence::DECLARED_SIZE_INCOMPLETE;
		}
		if (bytes.size() == Layout::DeclaredSize::END) {
			return FrameExistence::DECLARED_SIZE_COMPLETE;
		}
		if (bytes.size() < Layout::HEADER_SIZE) {
			return FrameExistence::INCOMPLETE;
		}
		return FrameExistence::YES;
	}

	template <KShared::Binary::ByteRange T> FrameExistence has_frame(const T &bytes) {
		const auto header = has_header(bytes);
		if (header != FrameExistence::YES) {
			return header;
		}

		const auto declared_size = get_declared_size(bytes);
		if (declared_size < Layout::MINIMUM_DECLARED_SIZE) {
			throw KShared::Binary::ByteError(
				std::format("Declared size {} is lower than the common frame header size {}",
							declared_size,
							Layout::MINIMUM_DECLARED_SIZE));
		}

		const auto frame_size = Layout::DeclaredSize::SIZE + declared_size;
		return bytes.size() >= frame_size ? FrameExistence::YES : FrameExistence::INCOMPLETE;
	}

} // namespace Karkinolution::Networking::Frames::Base::Parser
