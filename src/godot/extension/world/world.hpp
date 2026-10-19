#pragma once

#include "godot/extension/client/client.hpp"
#include "godot/extension/model/aliases.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "karkinolution/core/id.hpp"

namespace KarkinolutionGodot {
	namespace World {
		class GodotWorld : public godot::RefCounted {
				GDCLASS(GodotWorld, godot::RefCounted);

			private:

				godot::TypedArray<GodotCreature> creatures;
				godot::TypedArray<GodotCorpse>   corpses;

				bool sync_creature(BaseIdType id);
				bool sync_corpse(BaseIdType id);

				bool has_creature(BaseIdType id) const;
				bool has_corpse(BaseIdType id) const;

				bool update_creature(godot::Ref<GodotCreature> creature);
				bool update_corpse(godot::Ref<GodotCorpse> corpse);

				bool                            add(godot::Ref<GodotCreature> creature);
				bool                            add(godot::Ref<GodotCorpse> corpse);
				bool                            remove_creature(BaseIdType id);
				bool                            remove_corpse(BaseIdType id);
				bool                            add(BaseIdType id);
				godot::Ref<Client::GodotClient> client;

			protected:

				static void _bind_methods();

			public:

				GodotWorld() {
					client.instantiate();
				}

				bool has(BaseIdType id) const;

				void                             sync();
				godot::TypedArray<GodotCreature> get_all_creatures() const;
				godot::TypedArray<GodotCorpse>   get_all_corpses() const;
		};
	} // namespace World
} // namespace KarkinolutionGodot