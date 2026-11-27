#pragma once

#include "karkinolution/networking/frame/world/interpreters/creature.hpp"
#include "karkinolution/networking/frame/world/message_interpreters/models.hpp"
#include "shared/binary/fields.hpp"
#include "shared/core/id.hpp"

#include <array>
#include <vector>

namespace Karkinolution::Networking::Frames::World::MessageInterpreters {
	namespace SRI::Creature::Request {
		namespace Layout {
			namespace CreateCreature {
				// Payload: gender flag/value, species flag/value, position flag/Vec2, name flag/length/64-byte slot.
				using HasGender   = KShared::Binary::Field<0, 1>;
				using Gender      = KShared::Binary::Next<HasGender, 1>;
				using HasSpecies  = KShared::Binary::Next<Gender, 1>;
				using Species     = KShared::Binary::Next<HasSpecies, 1>;
				using HasPosition = KShared::Binary::Next<Species, 1>;
				using Position    = KShared::Binary::Next<HasPosition, VecSRI::Layout::TOTAL_SIZE>;
				using HasName     = KShared::Binary::Next<Position, 1>;
				using NameSize    = KShared::Binary::Next<HasName, 1>;
				using Name        = KShared::Binary::Next<NameSize, CreatureSRI::Layout::MAX_NAME_SIZE>;

				inline constexpr std::size_t TOTAL_SIZE = Name::END;
			} // namespace CreateCreature
		} // namespace Layout

		std::array<std::byte, 8> make_see_creature(KShared::Core::Id::BaseIdType id);
		std::array<std::byte, 1> make_see_all_creatures();
		std::array<std::byte, Layout::CreateCreature::TOTAL_SIZE>
		make_create_creature(const Models::Creature::CreaturePreset &preset);
	} // namespace SRI::Creature::Request

	namespace DSI::Creature::Request {
		KShared::Core::Id::BaseIdType    interpret_like_get_creature(const std::vector<std::byte> &payload);
		Models::Creature::CreaturePreset interpret_like_create_creature(const std::vector<std::byte> &payload);
	} // namespace DSI::Creature::Request
} // namespace Karkinolution::Networking::Frames::World::MessageInterpreters

namespace CreatureMessageSRI = Karkinolution::Networking::Frames::World::MessageInterpreters::SRI::Creature::Request;
namespace CreatureMessageDSI = Karkinolution::Networking::Frames::World::MessageInterpreters::DSI::Creature::Request;
