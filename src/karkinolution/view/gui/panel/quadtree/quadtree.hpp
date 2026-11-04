#pragma once
#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"
#include "karkinolution/view/gui/panel/quadtree/aabb.hpp"

#include <cstddef>
#include <karkinolution/view/gui/panel/quadtree/node.hpp>
#include <memory>
#include <optional>
#include <vector>

namespace KarkinolutionView {
	namespace Gui {
		namespace Panel {
			namespace Quadtree {
				class QuadtreeTestAccess;

				class Quadtree {
					private:

						friend class QuadtreeTestAccess;

						std::unique_ptr<Node> root;

						std::size_t size_ = 0;

						UpdateOutput update_implementation_(ComponentId id, const AABB &new_bound) {
							const auto output = root->update(id, new_bound);

							if (output == UpdateOutput::EXISTS_BUT_WAS_REMOVED) {
								--size_;
							}

							return output;
						}

						UpdateOutput update_implementation_(ComponentId id,
															const AABB &new_bound,
															const AABB &current_bound) {
							const auto output = root->update(id, new_bound);

							if (output == UpdateOutput::EXISTS_BUT_WAS_REMOVED) {
								--size_;
							}
							return output;
						}


					public:


						Quadtree(const Geometry::Rectangle &boundary)
							: root(std::make_unique<Node>(to_aabb(boundary), 0)) {}

						Quadtree(const Geometry::Circle &boundary)
							: root(std::make_unique<Node>(to_aabb(boundary), 0)) {}

						std::size_t size() const {
							return size_;
						}

						bool add(ComponentId id, const Geometry::Rectangle &form) {
							if (root->exists(id)) {
								return false;
							}

							const auto was_added =
								root->insert(Entry{.component_id = id, .bounds = to_aabb(form)});

							if (was_added) {
								++size_;
							}
							return was_added;
						}

						bool add(ComponentId id, const Geometry::Circle &form) {
							if (root->exists(id)) {
								return false;
							}

							const auto was_added =
								root->insert(Entry{.component_id = id, .bounds = to_aabb(form)});

							if (was_added) {
								++size_;
							}
							return was_added;
						}

						bool remove(ComponentId id) {
							const auto was_removed = root->remove(id);

							if (was_removed) {
								--size_;
							}
							return was_removed;
						}

						bool remove(ComponentId id, const Geometry::Rectangle &current_bound) {
							const auto was_removed = root->remove(id, to_aabb(current_bound));

							if (was_removed) {
								--size_;
							}
							return was_removed;
						}

						bool remove(ComponentId id, const Geometry::Circle &current_bound) {
							const auto was_removed = root->remove(id, to_aabb(current_bound));

							if (was_removed) {
								--size_;
							}
							return was_removed;
						}

						UpdateOutput update(ComponentId id, const Geometry::Rectangle &new_bound) {
							return update_implementation_(id, to_aabb(new_bound));
						}

						UpdateOutput update(ComponentId                id,
											const Geometry::Rectangle &new_bound,
											const Geometry::Rectangle &current_bound) {
							return update_implementation_(id,
														  to_aabb(new_bound),
														  to_aabb(current_bound));
						}

						UpdateOutput update(ComponentId id, const Geometry::Circle &new_bound) {
							return update_implementation_(id, to_aabb(new_bound));
						}

						UpdateOutput update(ComponentId             id,
											const Geometry::Circle &new_bound,
											const Geometry::Circle &current_bound) {
							return update_implementation_(id,
														  to_aabb(new_bound),
														  to_aabb(current_bound));
						}

						bool exists(ComponentId id) const {
							return root->exists(id);
						}

						bool exists(ComponentId id, Geometry::Rectangle &current_bound) const {
							return root->exists(id, to_aabb(current_bound));
						}

						bool exists(ComponentId id, Geometry::Circle &current_bound) const {
							return root->exists(id, to_aabb(current_bound));
						}

						std::optional<ComponentId>
						if_find_one_stop(const Geometry::Rectangle &area) const {
							return root->if_find_one_stop(to_aabb(area));
						}

						std::optional<ComponentId>
						if_find_one_stop(const Geometry::Circle &area) const {
							return root->if_find_one_stop(to_aabb(area));
						}

						std::vector<ComponentId>
						find_in_area(const Geometry::Rectangle &area) const {
							return root->find(to_aabb(area));
						}

						std::vector<ComponentId> find_in_area(const Geometry::Circle &area) const {
							return root->find(to_aabb(area));
						}
				};
			} // namespace Quadtree
		} // namespace Panel
	} // namespace Gui
} // namespace KarkinolutionView