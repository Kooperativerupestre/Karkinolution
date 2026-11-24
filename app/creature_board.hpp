#pragma once

#include "karkinolution/networking/frame/world/interpreters/models/creature.hpp"
#include "opsiamethystos/graphics/panel/components/board.hpp"

#include <optional>
#include <vector>

namespace KarkinolutionView {
	class CreatureBoard {
		private:

			using Creature = Karkinolution::Networking::Frames::World::Interpreters::Models::Creature::Creature;
			Opsiamethystos::Gui::Components::Board &board_;

			[[nodiscard]] static KShared::Colors::Color color_for(const Creature &creature);

		public:

			explicit CreatureBoard(Opsiamethystos::Gui::Components::Board &board)
				: board_(board) {}

			void draw(const std::vector<Creature>                     &creatures,
					  const std::optional<KShared::Math::Units::Size> &territory_size);
	};
} // namespace KarkinolutionView
