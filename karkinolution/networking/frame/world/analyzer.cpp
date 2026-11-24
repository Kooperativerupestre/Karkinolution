#include "karkinolution/networking/frame/world/aliases.hpp"
#include "karkinolution/networking/frame/world/core_aliases.hpp"

#include <karkinolution/networking/frame/world/analyzer.hpp>
#include <karkinolution/networking/frame/world/parser/parser.hpp>

bool Karkinolution::Networking::Frames::World::Analyzer::is_error(const std::vector<std::byte> &frame) {
	return Parser::Internal::get_type(frame) == BinaryTypes::Error;
}

bool Karkinolution::Networking::Frames::World::Analyzer::is_none(const std::vector<std::byte> &frame) {
	if (!is_response(frame)) {
		return false;
	}
	return std::get<ResponseSubTypes>(Parser::Internal::get_sub_type(frame)) == ResponseSubTypes::NONE;
}

bool Karkinolution::Networking::Frames::World::Analyzer::is_none(const ResponseFrame &frame) {
	return frame.get_sub_type() == ResponseSubTypes::NONE;
}

bool Karkinolution::Networking::Frames::World::Analyzer::is_response(const std::vector<std::byte> &frame) {
	return Parser::Internal::get_type(frame) == BinaryTypes::Response;
}

bool Karkinolution::Networking::Frames::World::Analyzer::is_request(const std::vector<std::byte> &frame) {
	return Parser::Internal::get_type(frame) == BinaryTypes::Request;
}