#include "karkinolution/binary/frames/aliases.hpp"

#include <karkinolution/binary/frames/motor.hpp>

ResponseFrame Karkinolution::Binary::Frames::Motor::make_none_response(RequestId request_id) {
	return make_response(BinaryTypes::Response,
						 ResponseSubTypes::NONE,
						 request_id,
						 std::vector<std::byte>{});
}