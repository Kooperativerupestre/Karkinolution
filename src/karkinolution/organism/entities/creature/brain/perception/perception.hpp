#pragma once

#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/terrain/rtree/box.hpp"

#include <functional>
#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <karkinolution/organism/nature/grass/grass.hpp>
#include <karkinolution/organism/nature/identity.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/terrain/terrain.hpp>
#include <optional>

using GeometryForms::Radius;
using OrganismStats::Body::RawMeat;

struct PerceivedSoil {
		SoilPieceId id;
		Vec3        position;
		Radius      radius;

		std::optional<SoilPieceComponents::Damage>       damage;
		std::optional<SoilPieceComponents::MovementCost> movement_cost;

		bool has_damage() const {
			return damage.has_value();
		}

		bool has_movement_cost() const {
			return movement_cost.has_value();
		}
};

struct PerceivedCreature {
		EntityId id;
		Energy   energy;
		Life     life;
		Gender   gender;
		Vec3     position;
		Size     size;
};

struct PerceivedCorpse {
		EntityId id;
		RawMeat  meat;
		Vec3     position;
		Size     size;
};

struct PerceivedGrass {
		NatureId    id;
		GrassMatter matter;
		Vec3        position;
		Radius      radius;
};

using PerceptionData  = RStarTree<std::variant<SoilPieceId, EntityId, NatureId>>;
using PerceivedEntity = std::variant<PerceivedCreature, PerceivedCorpse>;
using PerceivedNature = std::variant<PerceivedGrass>;

class PerceptionEntityRegistry : public BaseStorage<EntityId, PerceivedEntity> {
	public:

		using BaseStorage<EntityId, PerceivedEntity>::BaseStorage;
};

class PerceptionNatureRegistry : public BaseStorage<NatureId, PerceivedNature> {
	public:

		using BaseStorage<NatureId, PerceivedNature>::BaseStorage;
};

class PerceptionSoilRegistry : public BaseStorage<SoilPieceId, PerceivedSoil> {
	public:

		using BaseStorage<SoilPieceId, PerceivedSoil>::BaseStorage;
};

using EntityFilter = std::move_only_function<bool(const PerceivedEntity &)>;
using SoilFilter   = std::move_only_function<bool(const PerceivedSoil &)>;
using NatureFilter = std::move_only_function<bool(const PerceivedNature &)>;

class Perception;

namespace PerceivedEntityGetters {
	inline constexpr const Vec3 &get_position(const PerceivedEntity &entity) {
		return std::visit(
			[](const auto &ent) -> const Vec3 & {
				return ent.position;
			},
			entity);
	}

	inline constexpr const EntityId get_id(const PerceivedEntity &entity) {
		return std::visit(
			[](const auto &ent) -> EntityId {
				return ent.id;
			},
			entity);
	}

	inline constexpr const Size &get_size(const PerceivedEntity &entity) {
		return std::visit(
			[](const auto &ent) -> const Size & {
				return ent.size;
			},
			entity);
	}
} // namespace PerceivedEntityGetters

class PerceptionView {
	private:

		std::vector<SoilPieceId> soils_;
		std::vector<EntityId>    entities_;
		std::vector<NatureId>    natures_;

		std::optional<EntityFilter> entity_filter_;
		std::optional<SoilFilter>   soil_filter_;
		std::optional<NatureFilter> nature_filter_;

		const Perception* perception_;

	public:

		PerceptionView(const std::vector<SoilPieceId> soils,
					   const std::vector<EntityId>    entities,
					   const std::vector<NatureId>    natures,
					   const Perception              &perception,
					   std::optional<EntityFilter>    entity_filter = std::nullopt,
					   std::optional<SoilFilter> soil_filter        = std::nullopt,
					   std::optional<NatureFilter> nature_filter    = std::nullopt) {

			if (!entity_filter.has_value() && !soil_filter.has_value()
				&& !nature_filter.has_value()) {
				throw SimulationError("At least one filter should have value");
			}

			soils_    = soils;
			entities_ = entities;
			natures_  = natures;

			entity_filter_ = std::move(entity_filter);
			soil_filter_   = std::move(soil_filter);
			nature_filter_ = std::move(nature_filter);

			perception_ = &perception;
		}

		const Perception &perception() const {
			if (perception_ == nullptr) {
				throw SimulationError(
					"Invalid perception view: Original perception was be deleted");
			}
			return *perception_;
		}

		const std::vector<SoilPieceId> &soils() const {
			return soils_;
		}

		const std::vector<EntityId> &entities() const {
			return entities_;
		}

		const std::vector<NatureId> &natures() const {
			return natures_;
		}

		std::vector<std::reference_wrapper<const PerceivedSoil>>   resolved_soils() const;
		std::vector<std::reference_wrapper<const PerceivedEntity>> resolved_entities() const;
		std::vector<std::reference_wrapper<const PerceivedNature>> resolve_natures() const;

		bool exists(EntityId id) const {
			for (const auto &o_id : entities_) {
				if (o_id == id) {
					return true;
				}
			}
			return false;
		}

		bool exists(SoilPieceId id) const {
			for (const auto &o_id : soils_) {
				if (o_id == id) {
					return true;
				}
			}
			return false;
		}
};

class Perception {
	private:

		PerceptionData data_;
		Radius         radius_;

		PerceptionEntityRegistry entities_;
		PerceptionSoilRegistry   soils_;
		PerceptionNatureRegistry natures_;

		size_t entities_size_, soils_size_, natures_size_;
		Vec3   position_;
		Vec3   farthest_;

		double farthest_distance_;

	public:

		Perception(PerceptionData                 &data,
				   const Radius                   &radius,
				   const PerceptionEntityRegistry &entities,
				   const PerceptionSoilRegistry   &soils,
				   const PerceptionNatureRegistry &natures,
				   const Vec3                      position,
				   const Vec3                      farthest)
			: data_(std::move(data))
			, radius_(radius)
			, entities_(entities)
			, soils_(soils)
			, natures_(natures)
			, entities_size_(entities.size())
			, soils_size_(soils.size())
			, natures_size_(natures.size())
			, position_(position)
			, farthest_(farthest)
			, farthest_distance_(position.distance_to(farthest)) {}

		[[nodiscard]] const PerceptionData &data() const {
			return data_; // don't depend this if your goal isn't perception analyze.
		}

		[[nodiscard]] const PerceptionEntityRegistry &entities() const {
			return entities_;
		}

		[[nodiscard]] const PerceptionSoilRegistry &soils() const {
			return soils_;
		};

		[[nodiscard]] const PerceptionNatureRegistry &natures() const {
			return natures_;
		}

		[[nodiscard]] const Radius &radius() const {
			return radius_;
		};

		[[nodiscard]] size_t entities_size() const {
			return entities_size_;
		}

		[[nodiscard]] size_t soils_size() const {
			return soils_size_;
		}

		[[nodiscard]] size_t natures_size() const {
			return natures_size_;
		}

		[[nodiscard]] const Vec3 &farthest() const {
			return farthest_;
		}

		[[nodiscard]] const double farthest_distance() const {
			return farthest_distance_;
		}

		[[nodiscard]] const Vec3 &position() const {
			return position_;
		}

		[[nodiscard]] bool exists(EntityId id) const {
			return entities_.exists(id);
		}

		[[nodiscard]] bool exists(SoilPieceId id) const {
			return soils_.exists(id);
		}

		[[nodiscard]] bool exists(NatureId id) const {
			return natures_.exists(id);
		}

		[[nodiscard]] Box3D radius_box() const {
			return BoxConversion::to_box(radius_, position_);
		}
};

namespace PerceptionAnalyzer {

	PerceptionView filter(const Perception           &perception,
						  std::optional<EntityFilter> entity_filter = std::nullopt,
						  std::optional<SoilFilter> soil_filter     = std::nullopt,
						  std::optional<NatureFilter> nature_filter = std::nullopt);

	PerceptionView filter(const Perception               &perception,
						  const std::vector<EntityId>    &entities,
						  const std::vector<SoilPieceId> &soils,
						  std::optional<EntityFilter>     entity_filter = std::nullopt,
						  std::optional<SoilFilter> soil_filter         = std::nullopt,
						  std::optional<NatureFilter> nature_filter     = std::nullopt);

	PerceptionView filter(const PerceptionView       &view,
						  std::optional<EntityFilter> entity_filter = std::nullopt,
						  std::optional<SoilFilter> soil_filter     = std::nullopt,
						  std::optional<NatureFilter> nature_filter = std::nullopt);

	PerceptionView filter(const PerceptionView           &view,
						  const std::vector<EntityId>    &entities,
						  const std::vector<SoilPieceId> &soils,
						  std::optional<EntityFilter>     entity_filter = std::nullopt,
						  std::optional<SoilFilter> soil_filter         = std::nullopt,
						  std::optional<NatureFilter> nature_filter     = std::nullopt);

	PerceptionView filter_only_entities(const Perception &perception, EntityFilter filter);
	PerceptionView filter_only_entities(const PerceptionView &view, EntityFilter filter);

	PerceptionView filter_only_entities(const Perception            &perception,
										EntityFilter                 filter,
										const std::vector<EntityId> &entities);

	PerceptionView filter_only_entities(const PerceptionView        &view,
										EntityFilter                 filter,
										const std::vector<EntityId> &entities);

	PerceptionView filter_only_soils(const Perception &perception, SoilFilter filter);
	PerceptionView filter_only_soils(const PerceptionView &view, SoilFilter filter);

	PerceptionView filter_only_soils(const Perception               &perception,
									 SoilFilter                      filter,
									 const std::vector<SoilPieceId> &soils);

	PerceptionView filter_only_soils(const PerceptionView           &view,
									 SoilFilter                      filter,
									 const std::vector<SoilPieceId> &soils);

	PerceptionView filter_only_natures(const Perception &perception, NatureFilter filter);
	PerceptionView filter_only_natures(const PerceptionView &view, NatureFilter filter);

	PerceptionView filter_only_natures(const Perception            &perception,
									   NatureFilter                 filter,
									   const std::vector<NatureId> &natures);
	PerceptionView filter_only_natures(const PerceptionView        &view,
									   NatureFilter                 filter,
									   const std::vector<NatureId> &natures);

	// Utils
	PerceptionView reduce(const Perception           &perception,
						  const Radius               &radius,
						  std::optional<EntityFilter> entity_filter = std::nullopt,
						  std::optional<SoilFilter> soil_filter     = std::nullopt,
						  std::optional<NatureFilter> nature_filter = std::nullopt);

	PerceptionView reduce(const PerceptionView       &view,
						  const Radius               &radius,
						  std::optional<EntityFilter> entity_filter = std::nullopt,
						  std::optional<SoilFilter> soil_filter     = std::nullopt,
						  std::optional<NatureFilter> nature_filter = std::nullopt);

	NormalizedValue<double> normalize_distance(const Perception &perception, const Vec3 &position);
	NormalizedValue<double> normalize_distance(const PerceptionView &view, const Vec3 &poition);
} // namespace PerceptionAnalyzer