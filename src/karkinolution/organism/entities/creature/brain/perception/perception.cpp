#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/math/units.hpp"
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

std::vector<std::reference_wrapper<const PerceivedNature>> PerceptionView::resolve_natures() const {
	std::vector<std::reference_wrapper<const PerceivedNature>> r_natures;
	r_natures.reserve(natures_.size());

	for (auto nature_id : natures_) {
		r_natures.push_back(std::ref(perception_->natures().at(nature_id)));
	}
	return r_natures;
}

PerceptionView PerceptionAnalyzer::filter(const Perception           &perception,
										  std::optional<EntityFilter> entity_filter,
										  std::optional<SoilFilter>   soil_filter,
										  std::optional<NatureFilter> nature_filter) {
	std::vector<EntityId>    entities;
	std::vector<SoilPieceId> soils;
	std::vector<NatureId>    natures;

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

	if (nature_filter.has_value()) {
		for (const auto &[id, nature] : perception.natures().internal_data()) {
			if (nature_filter.value()(nature)) {
				natures.push_back(id);
			}
		}
	} else {
		for (const auto id : perception.natures().keys()) {
			natures.push_back(id);
		}
	}

	return PerceptionView(soils,
						  entities,
						  natures,
						  perception,
						  std::move(entity_filter),
						  std::move(soil_filter),
						  std::move(nature_filter));
}

PerceptionView PerceptionAnalyzer::filter(const Perception               &perception,
										  const std::vector<EntityId>    &entities,
										  const std::vector<SoilPieceId> &soils,
										  std::optional<EntityFilter>     entity_filter,
										  std::optional<SoilFilter>       soil_filter,
										  std::optional<NatureFilter>     nature_filter) {
	std::vector<EntityId>    v_entities;
	std::vector<SoilPieceId> v_soils;
	std::vector<NatureId>    v_natures;

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

	/*
	 * Nature filtering is intentionally analogous to entity and soil filtering.
	 * The explicit nature ID list is not part of this overload's signature,
	 * so all perceived natures are considered here.
	 */
	if (nature_filter.has_value()) {
		for (const auto &[id, nature] : perception.natures().internal_data()) {
			if (nature_filter.value()(nature)) {
				v_natures.push_back(id);
			}
		}
	} else {
		for (const auto id : perception.natures().keys()) {
			v_natures.push_back(id);
		}
	}

	return PerceptionView(v_soils,
						  v_entities,
						  v_natures,
						  perception,
						  std::move(entity_filter),
						  std::move(soil_filter),
						  std::move(nature_filter));
}

PerceptionView PerceptionAnalyzer::filter(const PerceptionView       &view,
										  std::optional<EntityFilter> entity_filter,
										  std::optional<SoilFilter>   soil_filter,
										  std::optional<NatureFilter> nature_filter) {
	return filter(view.perception(),
				  view.entities(),
				  view.soils(),
				  std::move(entity_filter),
				  std::move(soil_filter),
				  std::move(nature_filter));
}

PerceptionView PerceptionAnalyzer::filter(const PerceptionView           &view,
										  const std::vector<EntityId>    &entities,
										  const std::vector<SoilPieceId> &soils,
										  std::optional<EntityFilter>     entity_filter,
										  std::optional<SoilFilter>       soil_filter,
										  std::optional<NatureFilter>     nature_filter) {
	std::vector<EntityId>    new_entities;
	std::vector<SoilPieceId> new_soils;
	std::vector<NatureId>    new_natures;

	for (const auto id : view.entities()) {
		if (std::ranges::contains(entities, id)) {
			new_entities.push_back(id);
		}
	}

	for (const auto id : view.soils()) {
		if (std::ranges::contains(soils, id)) {
			new_soils.push_back(id);
		}
	}

	for (const auto id : view.natures()) {
		new_natures.push_back(id);
	}

	return filter(view.perception(),
				  new_entities,
				  new_soils,
				  std::move(entity_filter),
				  std::move(soil_filter),
				  std::move(nature_filter));
}

PerceptionView PerceptionAnalyzer::filter_only_entities(const Perception &perception,
														EntityFilter      filter) {
	return PerceptionAnalyzer::filter(perception, std::move(filter));
}

PerceptionView PerceptionAnalyzer::filter_only_entities(const PerceptionView &view,
														EntityFilter          filter) {
	return PerceptionAnalyzer::filter(view, std::move(filter));
}

PerceptionView PerceptionAnalyzer::filter_only_entities(const Perception            &perception,
														EntityFilter                 filter,
														const std::vector<EntityId> &entities) {
	std::vector<SoilPieceId> soils;

	for (const auto id : perception.soils().keys()) {
		soils.push_back(id);
	}

	return PerceptionAnalyzer::filter(perception, entities, soils, std::move(filter));
}

PerceptionView PerceptionAnalyzer::filter_only_entities(const PerceptionView        &view,
														EntityFilter                 filter,
														const std::vector<EntityId> &entities) {
	return PerceptionAnalyzer::filter(view, entities, view.soils(), std::move(filter));
}

PerceptionView PerceptionAnalyzer::filter_only_soils(const Perception &perception,
													 SoilFilter        filter) {
	return PerceptionAnalyzer::filter(perception, std::nullopt, std::move(filter));
}

PerceptionView PerceptionAnalyzer::filter_only_soils(const PerceptionView &view,
													 SoilFilter            filter) {
	return PerceptionAnalyzer::filter(view, std::nullopt, std::move(filter));
}

PerceptionView PerceptionAnalyzer::filter_only_soils(const Perception               &perception,
													 SoilFilter                      filter,
													 const std::vector<SoilPieceId> &soils) {
	std::vector<EntityId> entities;

	for (const auto id : perception.entities().keys()) {
		entities.push_back(id);
	}

	return PerceptionAnalyzer::filter(perception, entities, soils, std::nullopt, std::move(filter));
}

PerceptionView PerceptionAnalyzer::filter_only_soils(const PerceptionView           &view,
													 SoilFilter                      filter,
													 const std::vector<SoilPieceId> &soils) {
	return PerceptionAnalyzer::filter(view,
									  view.entities(),
									  soils,
									  std::nullopt,
									  std::move(filter));
}

PerceptionView PerceptionAnalyzer::filter_only_natures(const Perception &perception,
													   NatureFilter      filter) {
	return PerceptionAnalyzer::filter(perception, std::nullopt, std::nullopt, std::move(filter));
}

PerceptionView PerceptionAnalyzer::filter_only_natures(const PerceptionView &view,
													   NatureFilter          filter) {
	return PerceptionAnalyzer::filter(view, std::nullopt, std::nullopt, std::move(filter));
}

PerceptionView PerceptionAnalyzer::filter_only_natures(const Perception            &perception,
													   NatureFilter                 filter,
													   const std::vector<NatureId> &natures) {
	std::vector<NatureId> filtered_natures;

	for (const auto &[id, nature] : perception.natures().internal_data()) {
		if (std::ranges::contains(natures, id) && filter(nature)) {
			filtered_natures.push_back(id);
		}
	}

	std::vector<EntityId>    entities;
	std::vector<SoilPieceId> soils;

	for (const auto id : perception.entities().keys()) {
		entities.push_back(id);
	}

	for (const auto id : perception.soils().keys()) {
		soils.push_back(id);
	}

	return PerceptionView(soils,
						  entities,
						  filtered_natures,
						  perception,
						  std::nullopt,
						  std::nullopt,
						  std::move(filter));
}

PerceptionView PerceptionAnalyzer::filter_only_natures(const PerceptionView        &view,
													   NatureFilter                 filter,
													   const std::vector<NatureId> &natures) {
	std::vector<NatureId> filtered_natures;

	for (const auto id : view.natures()) {
		if (std::ranges::contains(natures, id)) {
			filtered_natures.push_back(id);
		}
	}

	return PerceptionView(view.soils(),
						  view.entities(),
						  filtered_natures,
						  view.perception(),
						  std::nullopt,
						  std::nullopt,
						  std::move(filter));
}

PerceptionView PerceptionAnalyzer::reduce(const Perception           &perception,
										  const Radius               &radius,
										  std::optional<EntityFilter> entity_filter,
										  std::optional<SoilFilter>   soil_filter,
										  std::optional<NatureFilter> nature_filter) {
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
		},
		std::move(nature_filter));
}

PerceptionView PerceptionAnalyzer::reduce(const PerceptionView       &view,
										  const Radius               &radius,
										  std::optional<EntityFilter> entity_filter,
										  std::optional<SoilFilter>   soil_filter,
										  std::optional<NatureFilter> nature_filter) {
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
		std::move(nature_filter));
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
