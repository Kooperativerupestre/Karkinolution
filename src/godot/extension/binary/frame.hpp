#pragma once

#include <cstdint>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>

struct ParsedFrame;

class GodotParsedFrame : public godot::RefCounted {
		GDCLASS(GodotParsedFrame, godot::RefCounted);

	private:

		std::uint32_t          size_{0};
		std::uint8_t           type_{0};
		std::uint32_t          sub_type_{0};
		godot::PackedByteArray payload_{};

	protected:

		static void _bind_methods();

	public:

		GodotParsedFrame();

		[[nodiscard]] std::uint32_t          get_size() const;
		void                                 set_size(std::uint32_t size);

		[[nodiscard]] std::uint8_t           get_type() const;
		void                                 set_type(std::uint8_t type);

		[[nodiscard]] std::uint32_t          get_sub_type() const;
		void                                 set_sub_type(std::uint32_t sub_type);

		[[nodiscard]] godot::PackedByteArray get_payload() const;
		void set_payload(const godot::PackedByteArray &payload);

		[[nodiscard]] bool                   is_request() const;
		[[nodiscard]] bool                   is_error() const;
		[[nodiscard]] bool                   is_response() const;

		[[nodiscard]] godot::String          get_payload_string() const;

		static godot::Ref<GodotParsedFrame>  from_core(const ::ParsedFrame &frame);
};
