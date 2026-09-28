#pragma once

#include "godot/extension/model/math/unit/units.hpp"

#include <cstdint>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/vector3.hpp>

struct DesserializedCorpse;
struct Corpse;

class GodotCorpse : public godot::RefCounted {
		GDCLASS(GodotCorpse, godot::RefCounted);

	private:

		std::uint64_t         id_{0};
		float                 raw_meat_{0.0f};
		godot::Vector3        position_{0.0, 0.0, 0.0};
		godot::Ref<GodotSize> size_;

	protected:

		static void _bind_methods();

	public:

		GodotCorpse();
		GodotCorpse(std::uint64_t                id,
					float                        raw_meat,
					const godot::Vector3        &position = godot::Vector3(),
					const godot::Ref<GodotSize> &size     = {});

		[[nodiscard]] std::uint64_t         get_id() const;
		[[nodiscard]] float                 get_raw_meat() const;
		[[nodiscard]] godot::Vector3        get_position() const;
		[[nodiscard]] godot::Ref<GodotSize> get_size() const;

		static godot::Ref<GodotCorpse> create(std::uint64_t         id          = 0,
											  float                 raw_meat    = 0.0f,
											  const godot::Vector3 &position    = godot::Vector3(),
											  const godot::Ref<GodotSize> &size = {});

		static godot::Ref<GodotCorpse> from_deserialized(const DesserializedCorpse &deserialized,
														 std::uint64_t              id = 0);
		static godot::Ref<GodotCorpse> from_core(const ::Corpse &corpse);
};
