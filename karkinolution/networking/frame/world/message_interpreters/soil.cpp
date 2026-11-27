#include "karkinolution/networking/frame/world/message_interpreters/soil.hpp"

#include "shared/binary/deserializer.hpp"
#include "shared/binary/serializer.hpp"

namespace Karkinolution::Networking::Frames::World::MessageInterpreters {
	namespace SRI::Soil::Request {
		std::array<std::byte, 8> make_see_soil(KShared::Core::Id::BaseIdType id) {
			return KShared::Binary::Serializer::serialize(id);
		}

		std::array<std::byte, 1> make_see_all_soils() {
			return {};
		}
	} // namespace SRI::Soil::Request

	namespace DSI::Soil::Request {
		KShared::Core::Id::BaseIdType interpret_like_get_soil(const std::vector<std::byte> &payload) {
			return KShared::Binary::Deserializer::deserialize<KShared::Core::Id::BaseIdType>(payload, 0);
		}
	} // namespace DSI::Soil::Request
} // namespace Karkinolution::Networking::Frames::World::MessageInterpreters
