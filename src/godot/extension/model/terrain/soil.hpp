#pragma once

#include "godot/extension/model/math/geometry/geometry.hpp"
#include "godot/extension/model/properties/properties.hpp"

#include <cstdint>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>

class GodotSoilPiece : public godot::RefCounted {
		GDCLASS(GodotSoilPiece, godot::RefCounted);

	public:

		enum SoilType {
			SAND  = 0,
			ROCK  = 1,
			DIRT  = 2,
			WATER = 3
		};

		enum SoilProperty {
			DANGEROUS = 0
		};

	protected:

		std::uint64_t  id_{0};
		std::uint8_t   type_{0};
		godot::Array   properties_;
		godot::Array   required_capabilities_;
		bool           has_damage_{false};
		float          damage_{0.0f};
		bool           has_movement_cost_{false};
		float          movement_cost_{0.0f};
		double         radius_{0.0};
		godot::Vector3 position_{0.0, 0.0, 0.0};

		static void _bind_methods();

	public:

		GodotSoilPiece();
		GodotSoilPiece(std::uint64_t         id,
					   std::uint8_t          type,
					   const godot::Array   &properties,
					   const godot::Array   &required_capabilities,
					   bool                  has_damage,
					   float                 damage,
					   bool                  has_movement_cost,
					   float                 movement_cost,
					   double                radius,
					   const godot::Vector3 &position);

		[[nodiscard]] std::uint64_t           get_id() const;
		[[nodiscard]] std::uint8_t            get_type() const;
		[[nodiscard]] godot::String           get_type_name() const;
		[[nodiscard]] godot::Array            get_properties() const;
		[[nodiscard]] godot::Array            get_required_capabilities() const;
		[[nodiscard]] bool                    has_damage() const;
		[[nodiscard]] float                   get_damage() const;
		[[nodiscard]] bool                    has_movement_cost() const;
		[[nodiscard]] float                   get_movement_cost() const;
		[[nodiscard]] double                  get_radius() const;
		[[nodiscard]] godot::Ref<GodotRadius> get_radius_ref() const;
		[[nodiscard]] godot::Vector3          get_position() const;

		static godot::Ref<GodotSoilPiece> create(std::uint64_t         id                    = 0,
												 std::uint8_t          type                  = 0,
												 const godot::Array   &properties            = {},
												 const godot::Array   &required_capabilities = {},
												 bool                  has_damage        = false,
												 float                 damage            = 0.0f,
												 bool                  has_movement_cost = false,
												 float                 movement_cost     = 0.0f,
												 double                radius            = 0.0,
												 const godot::Vector3 &position = godot::Vector3());
};

VARIANT_ENUM_CAST(GodotSoilPiece::SoilType);
VARIANT_ENUM_CAST(GodotSoilPiece::SoilProperty);
