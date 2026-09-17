#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/deserialization/interpreters/creature.hpp>
#include <karkinolution/binary/message_type_size.hpp>

BaseIdType CreatureDSI::interpret_like_get_creature(const std::vector<std::byte> &bytes) {

	return BaseIdType{Deserializer::read_uint64_t(bytes, 0)};
}