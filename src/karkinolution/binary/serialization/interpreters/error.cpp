#include "karkinolution/binary/serialization/serializer.hpp"

#include <cassert>
#include <karkinolution/binary/serialization/interpreters/error.hpp>

std::vector<std::byte> ErrorSRI::serialize_error(const std::string &error) {
	assert(error.size() <= ErrorSRI::ERROR_SIZE);
	return Serializer::convert_string(error);
}
