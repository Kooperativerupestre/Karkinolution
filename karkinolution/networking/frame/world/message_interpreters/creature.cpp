#include "karkinolution/networking/frame/world/message_interpreters/creature.hpp"

#include "shared/binary/byte_utils.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/errors.hpp"
#include "shared/binary/interpreters/vec.hpp"
#include "shared/binary/serializer.hpp"
#include "shared/core/error.hpp"

#include <format>

namespace Karkinolution::Networking::Frames::World::MessageInterpreters {
	namespace SRI::Creature::Request {
		std::array<std::byte, 8> make_see_creature(KShared::Core::Id::BaseIdType id) {
			return KShared::Binary::Serializer::serialize<KShared::Core::Id::BaseIdType>(id);
		}

		std::array<std::byte, 1> make_see_all_creatures() {
			return {};
		}

		std::array<std::byte, Layout::CreateCreature::TOTAL_SIZE>
		make_create_creature(const Models::Creature::CreaturePreset &preset) {
			if (preset.name && preset.name->size() > CreatureSRI::Layout::MAX_NAME_SIZE) {
				throw KShared::Errors::SimulationError(std::format("The max size of name is {}, received {}",
																   CreatureSRI::Layout::MAX_NAME_SIZE,
																   preset.name->size()));
			}

			std::array<std::byte, Layout::CreateCreature::TOTAL_SIZE> bytes{};

			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::convert_bool(preset.gender.has_value()),
												 Layout::CreateCreature::HasGender::OFFSET);
			if (preset.gender) {
				KShared::Binary::Utils::append_bytes(bytes,
													 CreatureSRI::serialize_gender(*preset.gender),
													 Layout::CreateCreature::Gender::OFFSET);
			}

			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::convert_bool(preset.specie.has_value()),
												 Layout::CreateCreature::HasSpecies::OFFSET);
			if (preset.specie) {
				KShared::Binary::Utils::append_bytes(bytes,
													 CreatureSRI::serialize_specie(*preset.specie),
													 Layout::CreateCreature::Species::OFFSET);
			}

			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::convert_bool(preset.position.has_value()),
												 Layout::CreateCreature::HasPosition::OFFSET);
			if (preset.position) {
				KShared::Binary::Utils::append_bytes(bytes,
													 VecSRI::serialize_vec(*preset.position),
													 Layout::CreateCreature::Position::OFFSET);
			}

			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::convert_bool(preset.name.has_value()),
												 Layout::CreateCreature::HasName::OFFSET);
			if (preset.name) {
				KShared::Binary::Utils::append_bytes(
					bytes,
					KShared::Binary::Serializer::convert_uint8_t(static_cast<std::uint8_t>(preset.name->size())),
					Layout::CreateCreature::NameSize::OFFSET);
				KShared::Binary::Utils::append_bytes(bytes,
													 KShared::Binary::Serializer::convert_string(*preset.name),
													 Layout::CreateCreature::Name::OFFSET);
			}

			return bytes;
		}
	} // namespace SRI::Creature::Request

	namespace DSI::Creature::Request {
		KShared::Core::Id::BaseIdType interpret_like_get_creature(const std::vector<std::byte> &payload) {
			return KShared::Binary::Deserializer::deserialize<KShared::Core::Id::BaseIdType>(payload, 0);
		}

		Models::Creature::CreaturePreset interpret_like_create_creature(const std::vector<std::byte> &payload) {
			namespace CreateCreature = SRI::Creature::Request::Layout::CreateCreature;

			Models::Creature::CreaturePreset preset;
			if (KShared::Binary::Deserializer::read_bool(payload, CreateCreature::HasGender::OFFSET)) {
				preset.gender = CreatureDSI::get_gender(payload, CreateCreature::Gender::OFFSET);
			}
			if (KShared::Binary::Deserializer::read_bool(payload, CreateCreature::HasSpecies::OFFSET)) {
				preset.specie = CreatureDSI::get_specie(payload, CreateCreature::Species::OFFSET);
			}
			if (KShared::Binary::Deserializer::read_bool(payload, CreateCreature::HasPosition::OFFSET)) {
				preset.position = VecDSI::get_vec(payload, CreateCreature::Position::OFFSET);
			}
			if (KShared::Binary::Deserializer::read_bool(payload, CreateCreature::HasName::OFFSET)) {
				const auto name_size =
					KShared::Binary::Deserializer::read_uint8_t(payload, CreateCreature::NameSize::OFFSET);
				if (name_size > CreatureSRI::Layout::MAX_NAME_SIZE) {
					throw KShared::Binary::ByteError(std::format("The max size of name is {}, received {}",
																 CreatureSRI::Layout::MAX_NAME_SIZE,
																 name_size));
				}
				preset.name =
					KShared::Binary::Deserializer::read_string(payload, CreateCreature::Name::OFFSET, name_size);
			}
			return preset;
		}
	} // namespace DSI::Creature::Request
} // namespace Karkinolution::Networking::Frames::World::MessageInterpreters
