#pragma once

#include "godot/extension/model/math/units.hpp"

#include <cstdint>
#include <godot/extension/model/math/aliases.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace KarkinolutionGodot {
	namespace Models {
		namespace Organisms {
			namespace Corpse {
				class GodotCorpse : public godot::RefCounted {
						GDCLASS(GodotCorpse, godot::RefCounted);

					private:

						std::uint64_t         id_;
						float                 raw_meat_;
						godot::Vector3        position_;
						godot::Ref<GodotSize> size_;

						GodotCorpse() {
							size_.instantiate();
						}

					protected:

						static void _bind_methods();

					public:


						GodotCorpse(std::int64_t                 id,
									float                        raw_meat,
									const godot::Vector3        &position,
									const godot::Ref<GodotSize> &size);

						[[nodiscard]] std::int64_t          get_id() const;
						[[nodiscard]] float                 get_raw_meat() const;
						[[nodiscard]] godot::Vector3        get_position() const;
						[[nodiscard]] godot::Ref<GodotSize> get_size() const;
				};
			} // namespace Corpse
		} // namespace Organisms
	} // namespace Models
} // namespace KarkinolutionGodot