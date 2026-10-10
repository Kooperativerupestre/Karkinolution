
#pragma once
#include <godot_cpp/variant/string.hpp>
#include <string>
#include <string_view>

inline godot::String to_godot_string(std::string_view s) {
	return godot::String::utf8(s.data(), (int64_t)s.size());
}

inline std::string from_godot_string(const godot::String &g) {
	godot::CharString cs = g.utf8();
	return std::string(cs.get_data(), (size_t)cs.length());
}