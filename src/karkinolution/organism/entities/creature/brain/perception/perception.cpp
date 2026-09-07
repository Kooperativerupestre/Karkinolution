#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/math/units.hpp"
#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/terrain/rtree/box.hpp"

#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/terrain/terrain.hpp>
#include <optional>
#include <vector>

std::vector<std::reference_wrapper<const PerceivedSoil>> PerceptionView::resolved_soils() const {
	std::vector<std::reference_wrapper<const PerceivedSoil>> r_soils;
	r_soils.reserve(soils_.size());

	for (auto soil_id : soils_) {
		r_soils.push_back(std::ref(perception_->soils().at(soil_id)));
	}
	return r_soils;
}

std::vector<std::reference_wrapper<const PerceivedEntity>>
PerceptionView::resolved_entities() const {
	std::vector<std::reference_wrapper<const PerceivedEntity>> r_entities;
	r_entities.reserve(entities_.size());

	for (auto entity_id : entities_) {
		r_entities.push_back(std::ref(perception_->entities().at(entity_id)));
	}
	return r_entities;
}

PerceptionView PerceptionAnalyzer::filter(const Perception           &perception,
										  std::optional<EntityFilter> entity_filter,
										  std::optional<SoilFilter>   soil_filter) {
	std::vector<Id>          entities;
	std::vector<SoilPieceId> soils;

	if (entity_filter.has_value()) {
		for (const auto &[id, entity] : perception.entities().internal_data()) {
			if (entity_filter.value()(entity)) {
				entities.push_back(id);
			}
		}
	} else {
		for (const auto id : perception.entities().keys()) {
			entities.push_back(id);
		}
	}

	if (soil_filter.has_value()) {
		for (const auto &[id, soil] : perception.soils().internal_data()) {
			if (soil_filter.value()(soil)) {
				soils.push_back(id);
			}
		}
	} else {
		for (const auto id : perception.soils().keys()) {
			soils.push_back(id);
		}
	}

	return PerceptionView(soils,
						  entities,
						  perception,
						  std::move(entity_filter),
						  std::move(soil_filter));
}

PerceptionView PerceptionAnalyzer::filter(const Perception               &perception,
										  std::optional<EntityFilter>     entity_filter,
										  std::optional<SoilFilter>       soil_filter,
										  const std::vector<Id>          &entities,
										  const std::vector<SoilPieceId> &soils) {
	std::vector<Id>          v_entities;
	std::vector<SoilPieceId> v_soils;

	if (entity_filter.has_value()) {
		for (const auto &[id, entity] : perception.entities().internal_data()) {
			if (std::ranges::contains(entities, id) && entity_filter.value()(entity)) {
				v_entities.push_back(id);
			}
		}
	} else {
		for (const auto id : perception.entities().keys()) {
			if (std::ranges::contains(entities, id)) {
				v_entities.push_back(id);
			}
		}
	}

	if (soil_filter.has_value()) {
		for (const auto &[id, soil] : perception.soils().internal_data()) {
			if (std::ranges::contains(soils, id) && soil_filter.value()(soil)) {
				v_soils.push_back(id);
			}
		}
	} else {
		for (const auto id : perception.soils().keys()) {
			if (std::ranges::contains(soils, id)) {
				v_soils.push_back(id);
			}
		}
	}

	return PerceptionView(v_soils,
						  v_entities,
						  perception,
						  std::move(entity_filter),
						  std::move(soil_filter));
}

PerceptionView PerceptionAnalyzer::filter(const PerceptionView       &view,
										  std::optional<EntityFilter> entity_filter,
										  std::optional<SoilFilter>   soil_filter) {
	return filter(view.perception(),
				  std::move(entity_filter),
				  std::move(soil_filter),
				  view.entities(),
				  view.soils());
}

PerceptionView PerceptionAnalyzer::filter(const PerceptionView           &view,
										  std::optional<EntityFilter>     entity_filter,
										  std::optional<SoilFilter>       soil_filter,
										  const std::vector<Id>          &entities,
										  const std::vector<SoilPieceId> &soils) {
	std::vector<Id>          new_entities;
	std::vector<SoilPieceId> new_soils;

	for (auto id : view.entities()) {
		if (std::ranges::contains(entities, id)) {
			new_entities.push_back(id);
		}
	}
	for (auto id : view.soils()) {
		if (std::ranges::contains(soils, id)) {
			new_soils.push_back(id);
		}
	}
	return filter(view.perception(),
				  std::move(entity_filter),
				  std::move(soil_filter),
				  new_entities,
				  new_soils);
}

PerceptionView PerceptionAnalyzer::reduce(const Perception           &perception,
										  const Radius               &radius,
										  std::optional<EntityFilter> entity_filter,
										  std::optional<SoilFilter>   soil_filter) {
	const auto radius_box = perception.radius_box();

	return filter(
		perception,
		[&radius_box](const PerceivedEntity &entity) {
			Vec3       position = PerceivedEntityGetters::get_position(entity);
			Size       size     = PerceivedEntityGetters::get_size(entity);
			const auto box      = BoxConversion::to_box(size, position);
			return Box3DMotor::intersect(radius_box, box);
		},
		[&radius_box](const PerceivedSoil &soil) {
			const auto box = BoxConversion::to_box(soil.radius, soil.position);
			return Box3DMotor::intersect(radius_box, box);
		});
}

PerceptionView PerceptionAnalyzer::reduce(const PerceptionView       &view,
										  const Radius               &radius,
										  std::optional<EntityFilter> entity_filter,
										  std::optional<SoilFilter>   soil_filter) {
	const auto radius_box = view.perception().radius_box();
	return filter(
		view,
		[&radius_box](const PerceivedEntity &entity) {
			Vec3       position = PerceivedEntityGetters::get_position(entity);
			Size       size     = PerceivedEntityGetters::get_size(entity);
			const auto box      = BoxConversion::to_box(size, position);
			return Box3DMotor::intersect(radius_box, box);
		},
		[&radius_box](const PerceivedSoil &soil) {
			const auto box = BoxConversion::to_box(soil.radius, soil.position);
			return Box3DMotor::intersect(radius_box, box);
		},
		view.entities(),
		view.soils());
}

NormalizedValue<double> PerceptionAnalyzer::normalize_distance(const Perception &perception,
															   const Vec3       &position) {
	return position.distance_to(perception.farthest()) / perception.farthest_distance();
}

NormalizedValue<double> PerceptionAnalyzer::normalize_distance(const PerceptionView &view,
															   const Vec3           &position) {
	return position.distance_to(view.perception().farthest())
		/ view.perception().farthest_distance();
}
