#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/frames/aliases.hpp"

#include <karkinolution/binary/frames/motor.hpp>

ResponseFrame Karkinolution::Binary::Frames::Motor::make_none_response(RequestId request_id) {
	return make_response(ResponseSubTypes::NONE,
						 request_id,
						 std::vector<std::byte>{},
						 ResponseCardinality::ONE);
}