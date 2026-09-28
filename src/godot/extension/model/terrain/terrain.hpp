#pragma once

#include "godot/extension/model/math/unit/units.hpp"
#include "godot/extension/model/terrain/soil.hpp"

#include <cstdint>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <unordered_map>

/*
 * GodotTerrain represents a fusion on the client/Godot side:
 * It fuses the Territory's dimensions (Size) and the collection of SoilPiece instances.
 *
 * NOTE: No spatial index (such as R*-tree or Octree) is replicated on the Godot client side.
 * Spatial performance bottlenecks, partitioning, and queries are strictly resolved server-side
 * in the simulation engine. The Godot client requires only storage, identification, and lookup
 * for rendering and client-side logic.
 */

class GodotTerrain : public godot::RefCounted {
		GDCLASS(GodotTerrain, godot::RefCounted);

	protected:

		godot::Ref<GodotSize>                                         size_;
		std::unordered_map<std::uint64_t, godot::Ref<GodotSoilPiece>> soils_;

		static void _bind_methods();

	public:

		GodotTerrain();
		explicit GodotTerrain(const godot::Ref<GodotSize> &size);

		[[nodiscard]] godot::Ref<GodotSize> get_size() const;
		void                                set_size(const godot::Ref<GodotSize> &size);

		[[nodiscard]] godot::Ref<GodotSoilPiece> get_soil(std::uint64_t id) const;
		[[nodiscard]] bool                       has_soil(std::uint64_t id) const;
		[[nodiscard]] godot::Array               list_soils() const;
		[[nodiscard]] godot::Array               list_ids() const;

		bool add_soil(const godot::Ref<GodotSoilPiece> &soil);
		bool remove_soil(std::uint64_t id);

		// Atomic update: If the soil does not exist, stops immediately and returns false.
		bool update_soil(std::uint64_t id, const godot::Ref<GodotSoilPiece> &soil);

		void                       clear();
		[[nodiscard]] std::int64_t size() const;

		static godot::Ref<GodotTerrain> create(const godot::Ref<GodotSize> &size = {});
};
