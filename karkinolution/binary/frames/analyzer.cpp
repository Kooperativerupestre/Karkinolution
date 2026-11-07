#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/frames/aliases.hpp"

#include <karkinolution/binary/frames/analyzer.hpp>
#include <karkinolution/binary/frames/parser.hpp>

bool Karkinolution::Binary::Frames::Analyzer::is_error(const std::vector<std::byte> &frame) {
	return Parser::get_type(frame) == BinaryTypes::Error;
}

bool Karkinolution::Binary::Frames::Analyzer::is_none(const std::vector<std::byte> &frame) {
	if (!is_response(frame)) {
		return false;
	}
	return std::get<ResponseSubTypes>(Parser::get_sub_type(frame)) == ResponseSubTypes::NONE;
}

bool Karkinolution::Binary::Frames::Analyzer::is_none(const ResponseFrame &frame) {
	return frame.get_sub_type() == ResponseSubTypes::NONE;
}

bool Karkinolution::Binary::Frames::Analyzer::is_response(const std::vector<std::byte> &frame) {
	return Parser::get_type(frame) == BinaryTypes::Response;
}

bool Karkinolution::Binary::Frames::Analyzer::is_request(const std::vector<std::byte> &frame) {
	return Parser::get_type(frame) == BinaryTypes::Request;
}