#include "karkinolution/binary/interpreters/error.hpp"

#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/serializer.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/core/id.hpp"

#include <cassert>
#include <format>

std::vector<std::byte> ErrorSRI::serialize_error(const std::string &error) {
	if (error.size() > ErrorSRI::Layout::ERROR_MAX_SIZE) {
		throw ByteError(std::format("Max error size is {}, {} isn't allowed",
									ErrorSRI::Layout::ERROR_MAX_SIZE,
									error.size()));
	};
	return Serializer::convert_string(error);
}

SEO ErrorSRI::serialize_creature_was_not_found(BaseIdType id) {
	return SEO{serialize_error(std::format("Creature with id {} was not found", id)),
			   ErrorSubTypes::CREATURE_WAS_NOT_FOUND};
}

SEO ErrorSRI::serialize_corpse_was_not_found(BaseIdType id) {
	return SEO{serialize_error(std::format("Corpse with id {} was not found", id)),
			   ErrorSubTypes::CORPSE_WAS_NOT_FOUND};
}
