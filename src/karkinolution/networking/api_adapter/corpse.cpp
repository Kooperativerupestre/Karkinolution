#include "karkinolution/binary/deserialization/interpreters/corpse/corpse.hpp"

#include "karkinolution/binary/core_aliases.hpp"

#include <karkinolution/api/corpse.hpp>
#include <karkinolution/binary/deserialization/interpreters/corpse/alias.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/serialization/interpreters/corpse/alias.hpp>
#include <karkinolution/networking/api_adapter/corpse.hpp>

ResponseFrame CorpseAPIBYA::see_corpse(const World                  &world,
									   RequestId                     request_id,
									   const std::vector<std::byte> &payload) {
	const auto id = CorpseDSI::Request::interpret_like_see_corpse(payload);

	auto corpse = Karkinolution::API::Organisms::Corpse::see_corpse(world, id);

	if (corpse == nullptr) {
		return Frames::Motor::make_response(BinaryTypes::Response,
											ResponseSubTypes ::NONE,
											request_id,
											std::vector<std::byte>{});
	}
	const auto corpse_payload = CorpseSRI::serialize_corpse(*corpse);

	return Frames::Motor::make_response(BinaryTypes::Response,
										ResponseSubTypes::CORPSE,
										request_id,
										corpse_payload);
}