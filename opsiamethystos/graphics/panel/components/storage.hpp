#pragma once

#include "opsiamethystos/graphics/panel/components/component.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace Opsiamethystos {
	namespace Graphics {
		namespace Panel {
			namespace Components {


				class ComponentsStorage {
					private:

						std::vector<std::shared_ptr<Gui::Components::Component>> components;

						bool is_one_taken_by_panel() const {
							for (size_t i = 0; i < components.size(); i++) {
								if (components[i].use_count() > 1) {
									return true;
								}
							}
							return false;
						}

					public:


						// If false is returned, it means that the Panel is using at least one component.
						// Components cannot be cleared if any of them is being used by a Panel.
						// O(n)
						bool clear() {
							if (is_one_taken_by_panel()) {
								return false;
							}

							components.clear();
							return true;
						}

						// If false is returned, it means that the Panel is using at least one component.
						// Components cannot be unemplaced if any of them is being used by a Panel.
						bool unemplace_all() {
							if (is_one_taken_by_panel()) {
								return false;
							}
							for (size_t i = 0; i < components.size(); i++) {
								components.at(i) = nullptr;
							}
							return true;
						}

						// O(1)
						bool has(std::size_t index) const {
							return components.size() > index;
						}

						// Returns false if the index does not exist. No UB.
						// O(1)
						bool is_emplaced(std::size_t index) const {
							if (!has(index)) {
								return false;
							}

							return components.at(index) != nullptr;
						}

						bool emplace(std::size_t index, Gui::Components::Component &&component) {
							if (!has(index)) {
								return false;
							}

							if (is_emplaced(index)) {
								return false;
							}

							components[index] = std::make_shared<Gui::Components::Component>(std::move(component));
							return true;
						}

						void reserve(std::size_t size) {
							components.reserve(size);
							components.resize(size);
						}

						void shrink_to_fit() {
							components.shrink_to_fit();
						}

						std::size_t size() const {
							return components.size();
						}

						std::vector<std::size_t> get_all_non_emplaced_indexes() const {
							std::vector<std::size_t> non_emplaced_indexes;
							for (size_t i = 0; i < components.size(); i++) {
								if (!is_emplaced(i)) {
									non_emplaced_indexes.push_back(i);
								}
							}
							return non_emplaced_indexes;
						}

						std::optional<std::shared_ptr<Gui::Components::Component>> share(std::size_t index) {
							if (!has(index)) {
								return std::nullopt;
							}
							return components.at(index);
						}

						ComponentsStorage() {}

						ComponentsStorage(std::size_t size) {
							reserve(size);
						}
				};
			} // namespace Components
		} // namespace Panel
	} // namespace Graphics
} // namespace Opsiamethystos