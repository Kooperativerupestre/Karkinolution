#pragma once
#include "karkinolution/math/geometry/models.hpp"

#include <cstdint>
#include <functional>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/organism/entities/properties/properties.hpp>
#include <karkinolution/organism/stats.hpp>
#include <karkinolution/terrain/rtree/box.hpp>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
using OrganismStats::Energy;

using GeometryForms::Radius;

struct Component {
		virtual ~Component()                    = default;
		Component()                             = default;
		Component(const Component &)            = default;
		Component &operator=(const Component &) = default;
		Component(Component &&)                 = default;
		Component &operator=(Component &&)      = default;
};

struct Components {
		std::unordered_map<std::type_index, std::unique_ptr<Component>> data;

		template <typename T> bool exists() const {
			return data.contains(std::type_index(typeid(T)));
		}

		template <typename T> bool add(T component) {
			if (exists<T>()) {
				return false;
			}

			data[std::type_index(typeid(T))] = std::make_unique<T>(std::move(component));

			return true;
		}

		bool add(std::unique_ptr<Component> component) {
			if (!component) {
				return false;
			}

			const auto component_type = std::type_index(typeid(*component));
			if (data.contains(component_type)) {
				return false;
			}

			data[component_type] = std::move(component);
			return true;
		}

		template <typename T> bool del() {
			return data.erase(std::type_index(typeid(T))) > 0;
		}

		template <typename T> T* try_get() const {
			auto it = data.find(std::type_index(typeid(T)));

			if (it == data.end()) {
				return nullptr;
			}

			return static_cast<T*>(it->second.get());
		}
};

enum class SoilTypes : uint8_t {
	SAND,
	ROCK,
	DIRT,
	WATER
};

enum class SoilProperties : uint8_t {
	EDIBLE,
	DANGEROUS
};

namespace SoilPieceComponents {

	struct FoodState : Component {
		public:

			Energy food;
			float  regen_tax;

			FoodState(Energy food, float regen_tax)
				: food(food)
				, regen_tax(regen_tax) {}
	};

	struct Damage : Component {
		public:

			float damage;

			Damage(float damage)
				: damage(damage) {}
	};

	struct MovementCost : Component {
		public:

			float cost;

			MovementCost(float cost)
				: cost(cost) {}
	};
} // namespace SoilPieceComponents

struct Blueprint {
		std::vector<std::function<std::unique_ptr<Component>()>> default_components;
		std::vector<SoilProperties>                              properties;
		std::vector<GenericProperty>                             required_properties;

		Components gen_components() const;
};

inline std::unordered_map<SoilTypes, Blueprint> blueprints = {
	{SoilTypes::DIRT,
	 Blueprint{.default_components = {
				   []() -> std::unique_ptr<Component> {
					   return std::make_unique<SoilPieceComponents::FoodState>(Energy(10.0f, 10.0f),
																			   1);
				   },
				   []() -> std::unique_ptr<Component> {
					   return std::make_unique<SoilPieceComponents::MovementCost>(1);
				   }},
			   .properties          = {SoilProperties::EDIBLE},
			   .required_properties = {Properties::Capabilities::Move::WALK}

	 }},

	{SoilTypes::SAND,
	 Blueprint{.default_components = {
				   []() -> std::unique_ptr<Component> {
					   return std::make_unique<SoilPieceComponents::FoodState>(Energy(10.0f, 10.0f),
																			   1);
				   },
				   []() -> std::unique_ptr<Component> {
					   return std::make_unique<SoilPieceComponents::MovementCost>(2);
				   }},
			   .properties = {SoilProperties::EDIBLE}}},

	{SoilTypes::ROCK,
	 Blueprint{.default_components  = {[]() -> std::unique_ptr<Component> {
                   return std::make_unique<SoilPieceComponents::MovementCost>(1);
               }},
			   .properties          = {SoilProperties::DANGEROUS},
			   .required_properties = {Properties::Capabilities::Move::WALK}}},

	{SoilTypes::WATER,
	 Blueprint{.default_components = {
				   []() -> std::unique_ptr<Component> {
					   return std::make_unique<SoilPieceComponents::FoodState>(Energy(10.0f, 10.0f),
																			   1);
				   },
				   []() -> std::unique_ptr<Component> {
					   return std::make_unique<SoilPieceComponents::MovementCost>(1);
				   }},
			   .properties          = {SoilProperties::EDIBLE},
			   .required_properties = {Properties::Capabilities::Move::SWIMM}}}};

using SoilPieceId = uint64_t;

struct SoilPiece {
		SoilTypes                    type;
		std::vector<SoilProperties>  properties;
		std::vector<GenericProperty> required_capabilities;
		Components                   components;

		Radius      radius;
		Vec3        position;
		SoilPieceId id;
};

namespace SoilF {
	SoilPiece
	gen_soil_piece(SoilTypes s_t, const GeometryForms::Radius &radius, const Vec3 &position);
} // namespace SoilF