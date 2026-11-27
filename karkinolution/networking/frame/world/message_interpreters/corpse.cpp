#include "karkinolution/networking/frame/world/message_interpreters/corpse.hpp"

#include "shared/binary/deserializer.hpp"
#include "shared/binary/serializer.hpp"

namespace Karkinolution::Networking::Frames::World::MessageInterpreters {
	namespace SRI::Corpse::Request {
		std::array<std::byte, 8> make_see_corpse(KShared::Core::Id::BaseIdType id) {
			return KShared::Binary::Serializer::serialize<KShared::Core::Id::BaseIdType>(id);
		}

		std::array<std::byte, 1> make_see_all_corpses() {
			return {};
		}
	} // namespace SRI::Corpse::Request

	namespace DSI::Corpse::Request {
		KShared::Core::Id::BaseIdType interpret_like_see_corpse(const std::vector<std::byte> &payload) {
			return KShared::Binary::Deserializer::deserialize<KShared::Core::Id::BaseIdType>(payload, 0);
		}
	} // namespace DSI::Corpse::Request
} // namespace Karkinolution::Networking::Frames::World::MessageInterpreters
