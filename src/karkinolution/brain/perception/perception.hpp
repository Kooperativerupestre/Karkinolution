#pragma once

#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/organism/foods/foods.hpp"
#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <optional>
struct PerceivedSoil {
    SoilPieceId id;
    Vec3 position;

    std::optional<Energy> food;
    std::optional<SoilPieceComponents::Damage> damage;
    std::optional<SoilPieceComponents::MovementCost> movement_cost;

    bool has_food() const { return food.has_value(); }
    bool has_damage() const { return damage.has_value(); }
    bool has_movement_cost() const { return movement_cost.has_value(); }
};

struct PerceivedCreature {
    Id id;
    Energy energy;
    Life life;
    Gender gender;
    Vec3 position;
};

struct PerceivedCorpse {
    Id id;
    RawMeat meat;
    Vec3 position;
};

using PerceptionData = RStarTree<std::variant<SoilPieceId, Id>>;
using PerceivedEntity = std::variant<PerceivedCreature, PerceivedCorpse>;

class PerceptionEntityRegistry : public BaseStorage<Id, PerceivedEntity> {
  public:
    using BaseStorage<Id, PerceivedEntity>::BaseStorage;
};

class PerceptionSoilRegistry : public BaseStorage<SoilPieceId, PerceivedSoil> {
  public:
    using BaseStorage<SoilPieceId, PerceivedSoil>::BaseStorage;
};

using EntityFilter = bool (*)(const PerceivedEntity&);
using SoilFilter = bool (*)(const PerceivedSoil&);

class Perception;

class PerceptionView {
  private:
    std::vector<SoilPieceId> soils_;
    std::vector<Id> entities_;

    std::optional<EntityFilter> entity_filter_;
    std::optional<SoilFilter> soil_filter_;

    const Perception* perception_;

  public:
    PerceptionView(const std::vector<SoilPieceId> soils, const std::vector<Id> entities,
                   const Perception& perception,
                   std::optional<EntityFilter> entity_filter = std::nullopt,
                   std::optional<SoilFilter> soil_filter = std::nullopt) {

        if (!entity_filter.has_value() && !soil_filter.has_value()) {
            throw SimulationError("At least one filter should have value");
        }

        soils_ = soils;
        entities_ = entities;

        entity_filter_ = entity_filter;
        soil_filter_ = soil_filter;

        perception_ = &perception;
    }

    const Perception& perception() const {
        if (perception_ == nullptr) {
            throw SimulationError("Invalid perception view: Original perception was be deleted");
        }
        return *perception_;
    }

    const std::vector<SoilPieceId>& soils() const { return soils_; }
    const std::vector<Id>& entities() const { return entities_; }

    bool exists(Id id) const {
        for (const auto& o_id : entities_) {
            if (o_id == id) {
                return true;
            }
        }
        return false;
    }

    bool exists(SoilPieceId id) const {
        for (const auto& o_id : soils_) {
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
    Radius radius_;

    PerceptionEntityRegistry entities_;
    PerceptionSoilRegistry soils_;

    size_t entities_size_, soils_size_;
    Vec3 position_;

  public:
    Perception(PerceptionData& data, const Radius& radius, const PerceptionEntityRegistry& entities,
               const PerceptionSoilRegistry& soils, const Vec3 position)
        : data_(std::move(data)), radius_(radius), entities_(entities), soils_(soils),
          entities_size_(entities.size()), soils_size_(soils.size()), position_(position) {}

    [[nodiscard]] const PerceptionData& data() const {
        return data_; // don't depend this if your goal isn't perception analyze.
    }

    [[nodiscard]] const PerceptionEntityRegistry& entities() const { return entities_; }
    [[nodiscard]] const PerceptionSoilRegistry& soils() const { return soils_; };

    [[nodiscard]] const Radius& radius() const { return radius_; };
    [[nodiscard]] size_t entities_size() const { return entities_size_; }

    [[nodiscard]] size_t soils_size() const { return soils_size_; }
    [[nodiscard]] const Vec3& position() const { return position_; }

    [[nodiscard]] bool exists(Id id) const { return entities_.exists(id); }

    [[nodiscard]] bool exists(SoilPieceId id) const { return soils_.exists(id); }
};

namespace PerceptionAnalyzer {

// Base

// Perception

PerceptionView filter(const Perception& perception,
                      std::optional<EntityFilter> entity_filter = std::nullopt,
                      std::optional<SoilFilter> soil_filter = std::nullopt);

PerceptionView filter(const Perception& perception, std::optional<EntityFilter> entity_filter,
                      std::optional<SoilFilter> soil_filter, const std::vector<Id>& entities,
                      const std::vector<SoilPieceId>& soils);

PerceptionView filter(const PerceptionView& view,
                      std::optional<EntityFilter> entity_filter = std::nullopt,
                      std::optional<SoilFilter> soil_filter = std::nullopt);

PerceptionView filter(const PerceptionView& view, std::optional<EntityFilter> entity_filter,
                      std::optional<SoilFilter> soil_filter, const std::vector<Id>& entities,
                      const std::vector<SoilPieceId>& soils);

PerceptionView filter_only_entities(const Perception& perception, EntityFilter filter);
PerceptionView filter_only_entities(const PerceptionView& view, EntityFilter filter);

PerceptionView filter_only_entities(const Perception& perception, EntityFilter filter,
                                    const std::vector<Id>& entities);

PerceptionView filter_only_entities(const PerceptionView& view, EntityFilter filter,
                                    const std::vector<Id>& entities);

PerceptionView filter_only_soils(const Perception& perception, SoilFilter filter);
PerceptionView filter_only_soils(const PerceptionView& view, SoilFilter filter);

PerceptionView filter_only_soils(const Perception& perception, SoilFilter filter,
                                 const std::vector<SoilPieceId>& soils);

PerceptionView filter_only_soils(const PerceptionView& view, SoilFilter filter,
                                 const std::vector<SoilPieceId>& soils);

// Utils
PerceptionView reduce(const Perception& perception, const Radius& radius,
                      std::optional<EntityFilter> entity_filter,
                      std::optional<SoilFilter> soil_filter);

PerceptionView reduce(const PerceptionView& view, const Radius& radius,
                      std::optional<EntityFilter> entity_filter = std::nullopt,
                      std::optional<SoilFilter> soil_filter = std::nullopt);

PerceptionView reperceive(const Perception& perception, const Radius& radius, const Vec3& position,
                          std::optional<EntityFilter> entity_filter = std::nullopt,
                          std::optional<SoilFilter> soil_filter = std::nullopt);

PerceptionView reperceive(const PerceptionView& view, const Radius& radius, const Vec3& position,
                          std::optional<EntityFilter> entity_filter = std::nullopt,
                          std::optional<SoilFilter> soil_filter = std::nullopt);
} // namespace PerceptionAnalyzer