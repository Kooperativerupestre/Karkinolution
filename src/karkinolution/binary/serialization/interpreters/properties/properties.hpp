#pragma once
#include "karkinolution/binary/fields.hpp"
#include "karkinolution/core/error.hpp"

#include <iostream>
#include <karkinolution/binary/serialization/serializer.hpp>
#include <karkinolution/organism/entities/properties/properties.hpp>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Properties {
					namespace Layout {
						using Kind = Field<0, 1>;
						using Type = Next<Kind, 1>;

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


					constexpr BytesArrays::Properties serialize(GenericProperty property) {
						BytesArrays::Properties bytes;

						bytes[Layout::Kind::OFFSET] =
							Serializer::convert_uint8_t(property.kind().value);


						bytes[Layout::Type::OFFSET] =
							Serializer::convert_uint8_t(property.raw().value);

						return bytes;
					}
				} // namespace Properties
			} // namespace SRI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
