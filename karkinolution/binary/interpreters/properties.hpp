#pragma once

#include "karkinolution/organism/entities/properties/properties.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/errors.hpp"
#include "shared/binary/fields.hpp"
#include "shared/binary/serializer.hpp"

#include <array>
#include <format>
#include <iostream>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Properties {
					namespace Layout {
						using Kind = KShared::Binary::Field<0, 1>;
						using Type = KShared::Binary::Next<Kind, 1>;

						inline constexpr std::size_t TOTAL_SIZE = Type::END;
					} // namespace Layout

					namespace BytesArrays {
						using Properties = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArrays

					/*
					protocol

					[1 byte - kind (variant index)]
					[1 byte - value]

					*/

					constexpr BytesArrays::Properties
					serialize(Karkinolution::Organisms::Entities::GenericProperty property) {
						BytesArrays::Properties bytes;

						bytes[Layout::Kind::OFFSET] =
							KShared::Binary::Serializer::convert_uint8_t(property.kind().value);

						bytes[Layout::Type::OFFSET] =
							KShared::Binary::Serializer::convert_uint8_t(property.raw().value);

						return bytes;
					}
				} // namespace Properties
			} // namespace SRI

			namespace DSI {
				namespace Properties {
					template <KShared::Binary::ByteRange T>
					Karkinolution::Organisms::Entities::GenericProperty constexpr deserialize(const T    &payload,
																							  std::size_t offset) {
						Karkinolution::Organisms::Entities::Properties::Kind kind{
							KShared::Binary::Deserializer::read_uint8_t(payload,
																		SRI::Properties::Layout::Kind::OFFSET
																			+ offset)};

						Karkinolution::Organisms::Entities::Properties::Type type(
							KShared::Binary::Deserializer::read_uint8_t(payload,
																		SRI::Properties::Layout::Type::OFFSET
																			+ offset));
						auto property = Karkinolution::Organisms::Entities::GenericProperty::fromRaw(kind, type);
						if (!property) {
							throw KShared::Binary::ByteError(
								std::format("Invalid combination of kind {} and type {}", kind.value, type.value));
						}
						return property.value();
					}
				} // namespace Properties
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace PropertiesSRI = Karkinolution::Binary::Interpreters::SRI::Properties;
namespace PropertiesDSI = Karkinolution::Binary::Interpreters::DSI::Properties;
