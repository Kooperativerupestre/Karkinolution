#include "karkinolution/binary/frames/aliases.hpp"

#include <karkinolution/binary/frames/motor.hpp>

ResponseFrame Frames::Motor::make_none_response(Frames::Types::RequestId request_id) {
	return make_response(BinaryTypes::Response,
						 ResponseSubTypes::NONE,
						 request_id,
						 std::vector<std::byte>{});
}