#include "karkinolution/networking/raw_frame_buffer.hpp"

#include "karkinolution/networking/frame/system/layout.hpp"
#include "karkinolution/networking/frame/world/protocol/layouts.hpp"
#include "karkinolution/networking/scope/layout.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

#include <cstdint>
#include <karkinolution/networking/frame/system/parser/parser.hpp>
#include <karkinolution/networking/frame/world/parser/parser.hpp>
#include <vector>

namespace Karkinolution::Networking {
	BufferExistence FrameBufferAnalyzer::has_frame(const std::vector<std::byte> &bytes, Scope::Scope scope) {
		if (scope == Scope::Scope::WORLD) {
			return Frames::World::Parser::has_frame(bytes);
		} else if (scope == Scope::Scope::SERVER) {
			return Frames::System::Parser::has_frame(bytes);
		}
		KShared::Functional::unreachable_or_throw();
	}

	BufferExistence FrameBufferAnalyzer::has_frame(const RawFrameBuffer &buffer, Scope::Scope scope) {
		if (scope == Scope::Scope::WORLD) {
			return Frames::World::Parser::has_frame(buffer.view());
		} else if (scope == Scope::Scope::SERVER) {
			return Frames::System::Parser::has_frame(buffer.view());
		}
		KShared::Functional::unreachable_or_throw();
	}

	std::optional<std::vector<std::byte>> FrameBufferAnalyzer::get_frame(const RawFrameBuffer &buffer,
																		 Scope::Scope          scope) {

		if (has_frame(buffer, scope) != BufferExistence::YES) {
			return std::nullopt;
		}
		if (scope == Scope::Scope::WORLD) {
			return Frames::World::Parser::get_frame(buffer.view());
		} else if (scope == Scope::Scope::SERVER) {
			return Frames::System::Parser::get_frame(buffer.view());
		}
		KShared::Functional::unreachable_or_throw();
	}

	std::uint32_t FrameBufferAnalyzer::get_size_of_frame(const RawFrameBuffer &buffer, Scope::Scope scope) {
		if (scope == Scope::Scope::SERVER) {
			return Frames::System::Parser::get_frame_size(buffer.view());
		} else if (scope == Scope::Scope::WORLD) {
			return Frames::World::Parser::get_frame_size(buffer.view());
		}
		KShared::Functional::unreachable_or_throw();
	}

	std::uint32_t FrameBufferAnalyzer::get_size_of_frame(const std::vector<std::byte> &declared_size,
														 Scope::Scope                  scope) {
		if (scope == Scope::Scope::SERVER) {
			return Frames::System::Parser::get_frame_size(declared_size);
		} else if (scope == Scope::Scope::WORLD) {
			return Frames::World::Parser::get_frame_size(declared_size);
		}
		KShared::Functional::unreachable_or_throw();
	}

	std::optional<std::size_t> FrameBufferAnalyzer::get_end_of_frame(const RawFrameBuffer &buffer, Scope::Scope scope) {
		const auto has_frame_output = has_frame(buffer, scope);
		if (has_frame_output != BufferExistence::YES) {
			return std::nullopt;
		}

		return get_size_of_frame(buffer, scope);
	}

	std::optional<std::size_t> FrameBufferAnalyzer::get_end_of_frame(const std::vector<std::byte> &bytes,
																	 Scope::Scope                  scope) {
		const auto has_frame_output = has_frame(bytes, scope);

		if (has_frame_output != BufferExistence::YES) {
			return std::nullopt;
		}
		return get_size_of_frame(bytes, scope);
	}

	bool has_declared_size(std::size_t buffer_size, std::size_t new_bytes_size, Scope::Scope scope) {
		if (scope == Scope::Scope::SERVER) {
			return buffer_size + new_bytes_size >= Frames::System::Layout::DeclaredSize::END;
		} else if (scope == Scope::Scope::WORLD) {
			return buffer_size + new_bytes_size >= Frames::World::ProtocolLayout::DeclaredSize::END;
		}
		KShared::Functional::unreachable_or_throw();
	}

	bool has_declared_size(std::size_t buffer_size, Scope::Scope scope) {
		if (scope == Scope::Scope::SERVER) {
			return buffer_size >= Frames::System::Layout::DeclaredSize::END;
		} else if (scope == Scope::Scope::WORLD) {
			return buffer_size >= Frames::World::ProtocolLayout::DeclaredSize::END;
		}
		KShared::Functional::unreachable_or_throw();
	}

	std::optional<FrameBufferAnalyzer::EndOfFrame>
	FrameBufferAnalyzer::get_end_of_frame(const RawFrameBuffer      &buffer,
										  std::span<const std::byte> new_bytes,
										  Scope::Scope               scope) {
		const auto buffer_try = get_end_of_frame(buffer, scope);
		// First, it uses the other overload of the function to check whether the frame is already complete in the
		// buffer.
		if (buffer_try) {
			return EndOfFrame{.index = buffer_try.value(), .place = PlacesOfEndOfFrame::BUFFER};
		}

		// Here, we check whether the declared size has been fully received, by checking if the buffer combined with the
		// new bytes is enough to complete it.

		if (!has_declared_size(buffer.view().size(), new_bytes.size(), scope)) {
			return std::nullopt;
		}

		std::vector<std::byte> bytes = {};
		bytes.reserve(sizeof(uint32_t));

		if (has_declared_size(buffer.view().size(), scope)) {
			// If the declared size is already fully present in the buffer, we only retrieve the bytes needed to
			// construct it, avoiding unnecessary copies.
			bytes.insert(bytes.end(), buffer.view().begin(), buffer.view().begin() + sizeof(uint32_t));

		} else {
			// If the declared size is not present, the buffer contains fewer bytes than needed to construct it, so we
			// can simply append all of them. However, we must also append the necessary bytes from new_bytes so that
			// they are included as well.
			bytes.insert(bytes.end(), buffer.view().begin(), buffer.view().end());
			bytes.insert(bytes.end(), new_bytes.begin(), new_bytes.begin() + (sizeof(uint32_t) - buffer.view().size()));
		}

		assert(bytes.size() == 4);

		const auto size_of_frame    = get_size_of_frame(bytes, scope);
		const auto accumulated_size = buffer.view().size() + new_bytes.size();


		if (accumulated_size < size_of_frame) {
			return std::nullopt;
		}

		// Logically, if the frame starts in the buffer but does not end there, the buffer size must be smaller than
		// the total frame size.
		assert(size_of_frame > buffer.view().size());
		// The difference between the frame size and the buffer size represents the index in new_bytes where the frame
		// ends.
		return EndOfFrame{.index = size_of_frame - buffer.view().size(), .place = PlacesOfEndOfFrame::NEW_BYTES};
	}
} // namespace Karkinolution::Networking