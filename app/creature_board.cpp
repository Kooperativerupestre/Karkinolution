#include "app/creature_board.hpp"

#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "shared/core/color.hpp"

#include <algorithm>

namespace KarkinolutionView {
	namespace Geometry = Opsiamethystos::Math::Geometry;
	namespace Colors   = KShared::Colors;

	Colors::Color CreatureBoard::color_for(const Creature &creature) {
		using Species = Karkinolution::Organisms::Entities::Creature::CreatureSpecies;
		switch (creature.specie) {
			case Species::CRAB:
				return Colors::Presets::Red;
			case Species::FISH:
				return Colors::Presets::Blue;
			case Species::CROCODILE:
				return Colors::Presets::Green;
			case Species::HIPPOPOTAMUS:
				return Colors::Presets::Purple;
		}
		return Colors::Presets::White;
	}

	void CreatureBoard::draw(const std::vector<Creature>                     &creatures,
							 const std::optional<KShared::Math::Units::Size> &territory_size) {
		board_.clear();
		const auto bounds = board_.get_bounds();
		if (!board_.draw_rectangle(bounds, Colors::Presets::Black)) {
			return;
		}
		if (!territory_size || territory_size->width.value <= 0.0 || territory_size->height.value <= 0.0) {
			return;
		}

		const float half_board_width  = static_cast<float>(bounds.width.value()) / 2.0F;
		const float half_board_height = static_cast<float>(bounds.height.value()) / 2.0F;
		const float half_world_width  = static_cast<float>(territory_size->width.value) / 2.0F;
		const float half_world_height = static_cast<float>(territory_size->height.value) / 2.0F;

		for (const auto &creature : creatures) {
			const Geometry::Position position{
				static_cast<float>(creature.position.x / half_world_width * half_board_width),
				static_cast<float>(-creature.position.y / half_world_height * half_board_height)};
			const float width_radius = static_cast<float>(creature.morphology.size.width.value)
				/ static_cast<float>(territory_size->width.value) * static_cast<float>(bounds.width.value());
			const float height_radius = static_cast<float>(creature.morphology.size.height.value)
				/ static_cast<float>(territory_size->height.value) * static_cast<float>(bounds.height.value());
			const float radius = std::clamp(std::max(width_radius, height_radius) / 2.0F, 4.0F, 18.0F);
			board_.draw_circle(Geometry::Circle{position, Geometry::Radius{radius}}, color_for(creature));
		}
	}
} // namespace KarkinolutionView
