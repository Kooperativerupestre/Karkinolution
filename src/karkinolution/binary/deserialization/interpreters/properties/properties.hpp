#pragma once
#include "karkinolution/binary/byte_range.hpp"
#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/binary/serialization/interpreters/properties/properties.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/organism/entities/properties/properties.hpp"

#include <karkinolution/binary/serialization/interpreters/properties/alias.hpp>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace DSI {
				namespace Properties {
					template <ByteRange T>
					GenericProperty constexpr deserialize(const T &payload, std::size_t offset) {
						::Properties::Kind kind{Deserializer::read_uint8_t(
							payload,
							PropertiesSRI::Layout::Kind::OFFSET + offset)};

						::Properties::Type type(Deserializer::read_uint8_t(
							payload,
							PropertiesSRI::Layout::Type::OFFSET + offset));
						auto               property = GenericProperty::fromRaw(kind, type);
						if (!property) {
							throw ByteError(
								std::format("Invalid combination of kind {} and type {}",
											kind.value,
											type.value));
						}
						return property.value();
					}
				} // namespace Properties
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
