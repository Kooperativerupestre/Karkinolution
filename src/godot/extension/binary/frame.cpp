#include "godot/extension/binary/frame.hpp"

#include <cstring>
#include <karkinolution/binary/frames/parser.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <variant>

void GodotParsedFrame::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("get_size"), &GodotParsedFrame::get_size);
	godot::ClassDB::bind_method(godot::D_METHOD("set_size", "size"), &GodotParsedFrame::set_size);
	godot::ClassDB::add_property("GodotParsedFrame",
								 godot::PropertyInfo(godot::Variant::INT, "size"),
								 "set_size",
								 "get_size");

	godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &GodotParsedFrame::get_type);
	godot::ClassDB::bind_method(godot::D_METHOD("set_type", "type"), &GodotParsedFrame::set_type);
	godot::ClassDB::add_property("GodotParsedFrame",
								 godot::PropertyInfo(godot::Variant::INT, "type"),
								 "set_type",
								 "get_type");

	godot::ClassDB::bind_method(godot::D_METHOD("get_sub_type"), &GodotParsedFrame::get_sub_type);
	godot::ClassDB::bind_method(godot::D_METHOD("set_sub_type", "sub_type"),
								&GodotParsedFrame::set_sub_type);
	godot::ClassDB::add_property("GodotParsedFrame",
								 godot::PropertyInfo(godot::Variant::INT, "sub_type"),
								 "set_sub_type",
								 "get_sub_type");

	godot::ClassDB::bind_method(godot::D_METHOD("get_payload"), &GodotParsedFrame::get_payload);
	godot::ClassDB::bind_method(godot::D_METHOD("set_payload", "payload"),
								&GodotParsedFrame::set_payload);
	godot::ClassDB::add_property("GodotParsedFrame",
								 godot::PropertyInfo(godot::Variant::PACKED_BYTE_ARRAY, "payload"),
								 "set_payload",
								 "get_payload");

	godot::ClassDB::bind_method(godot::D_METHOD("is_request"), &GodotParsedFrame::is_request);
	godot::ClassDB::bind_method(godot::D_METHOD("is_error"), &GodotParsedFrame::is_error);
	godot::ClassDB::bind_method(godot::D_METHOD("is_response"), &GodotParsedFrame::is_response);
	godot::ClassDB::bind_method(godot::D_METHOD("get_payload_string"),
								&GodotParsedFrame::get_payload_string);
}

GodotParsedFrame::GodotParsedFrame() {}

std::uint32_t GodotParsedFrame::get_size() const {
	return size_;
}

void GodotParsedFrame::set_size(std::uint32_t size) {
	size_ = size;
}

std::uint8_t GodotParsedFrame::get_type() const {
	return type_;
}

void GodotParsedFrame::set_type(std::uint8_t type) {
	type_ = type;
}

std::uint32_t GodotParsedFrame::get_sub_type() const {
	return sub_type_;
}

void GodotParsedFrame::set_sub_type(std::uint32_t sub_type) {
	sub_type_ = sub_type;
}

godot::PackedByteArray GodotParsedFrame::get_payload() const {
	return payload_;
}

void GodotParsedFrame::set_payload(const godot::PackedByteArray &payload) {
	payload_ = payload;
}

bool GodotParsedFrame::is_request() const {
	return type_ == static_cast<std::uint8_t>(BinaryTypes::Request);
}

bool GodotParsedFrame::is_error() const {
	return type_ == static_cast<std::uint8_t>(BinaryTypes::Error);
}

bool GodotParsedFrame::is_response() const {
	return type_ == static_cast<std::uint8_t>(BinaryTypes::Response);
}

godot::String GodotParsedFrame::get_payload_string() const {
	return payload_.get_string_from_utf8();
}

godot::Ref<GodotParsedFrame> GodotParsedFrame::from_core(const ::ParsedFrame &frame) {
	godot::Ref<GodotParsedFrame> result;
	result.instantiate();
	result->set_size(frame.size);
	result->set_type(static_cast<std::uint8_t>(frame.type));
	result->set_sub_type(std::visit(
		[](auto code) {
			return static_cast<std::uint32_t>(code);
		},
		frame.sub_type));

	godot::PackedByteArray payload;
	payload.resize(static_cast<std::int64_t>(frame.payload.size()));
	if (!frame.payload.empty()) {
		std::memcpy(payload.ptrw(), frame.payload.data(), frame.payload.size());
	}
	result->set_payload(payload);

	return result;
}
