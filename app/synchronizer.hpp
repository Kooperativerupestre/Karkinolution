#pragma once

#include "app/aliases.hpp"
#include "app/commanders/concepts.hpp"
#include "opsiamethystos/graphics/panel/components/component.hpp"
#include "opsiamethystos/graphics/panel/components/low_level/button.hpp"
#include "opsiamethystos/graphics/panel/layout/layout.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"
#include "shared/functional/optional_reference.hpp"

#include <app/commanders/creature.hpp>
#include <app/world_environment.hpp>
#include <cstddef>
#include <iostream>
#include <karkinolution/networking/client_side/world/world.hpp>
#include <karkinolution/networking/client_side/world/worlds_manager.hpp>
#include <memory>
#include <opsiamethystos/graphics/models/models.hpp>
#include <string>

namespace KApp {


	class Viewer {
		private:

			WorldEnvironment &environment;

			ComponentsStorage components;

			Panel &panel;

			void reserve_components() {

				// Connection status label -> Label
				components.emplace(
					0,
					Component{Label{Math::Geometry::Rectangle{Math::Geometry::Position{0.0f, 0.0f}, 160, 160},
									"Disconnected",
									KShared::Colors::Presets::White,
									KShared::Colors::Presets::Green,
									20}});

				// Connect/Disconnect button -> RectangleButton
				components.emplace(
					1,
					Component{Button{Math::Geometry::Rectangle{Math::Geometry::Position{0.0f, 0.0f}, 100, 100},
									 [this]() {
										 environment.connect();
									 },
									 ButtonStyles{KShared::Colors::Presets::White,
												  KShared::Colors::Presets::DarkGray,
												  KShared::Colors::Presets::Gray},
									 "Connect",
									 KShared::Colors::Presets::Green,
									 20}});

				// Create creature button


				components.emplace(
					2,
					Component{Button{Math::Geometry::Rectangle{Math::Geometry::Position{0.0f, 0.0f}, 100, 100},
									 [this]() {
										 environment.throw_if_no_connected(
											 "To make a creature, the environment should be connected");
										 Commanders::Creature::make_creature(
											 environment,
											 environment.get_current_world().reference());
									 },
									 ButtonStyles{KShared::Colors::Presets::White,
												  KShared::Colors::Presets::Gray,
												  KShared::Colors::Presets::Purple},
									 "Make creature",
									 KShared::Colors::Presets::Gold,
									 20}});
			}

			template <typename T> T &get_component(std::size_t index) {
				return std::get<T>(components.share(index).value()->get_data());
			}

			void update_components() {
				// slot 0 -> connection status
				{
					auto &status = get_component<Label>(0);
					if (environment.is_connected()) {

						status.set_text("Connected");
						status.set_text_color(KShared::Colors::Presets::Green);
					}

					else {
						status.set_text("Disconnected");
						status.set_text_color(KShared::Colors::Presets::Red);
					}
				}

				// slot 1 -> connect/disconnect button

				{
					auto &button = get_component<Button>(1);

					if (environment.is_connected()) {
						button.set_text("Disconnect");
						button.set_text_color(KShared::Colors::Presets::Red);
					} else {
						button.set_text("Connect");
						button.set_text_color(KShared::Colors::Presets::Green);
					}
				}
			}

		public:

			Viewer(WorldEnvironment &environment, Panel &panel)
				: environment(environment)
				, components(10)
				, panel(panel) {
				reserve_components();

				panel.add(components.share(0).value());
				panel.add(components.share(1).value());
				panel.add(components.share(2).value());

				update_components();

				namespace Layout = Opsiamethystos::Gui::Panel::Layout;
				namespace Font   = Opsiamethystos::Gui::Components::Drivers::Font;

				const auto connection_status_label_id = components.share(0).value()->get_id();

				const auto connect_button_node =
					std::make_shared<const Layout::Node>(Layout::Node{Layout::leaf(components.share(1).value())});

				const auto make_creature_button_node =
					std::make_shared<const Layout::Node>(Layout::Node{Layout::leaf(components.share(2).value())});

				const auto label_node = std::make_shared<const Layout::Node>(
					Layout::Node{Layout::Leaf{connection_status_label_id, [] {
												  const auto widest = Font::measure_text("Disconnected", 20);
												  return Layout::Size{widest.width, 100};
											  }}});

				panel.set_layout(
					Layout::Node{Layout::Stack{.direction   = Layout::Direction::HORIZONTAL,
											   .spacing     = 10,
											   .cross_align = Layout::CrossAlign::START,
											   .children    = {{Layout::Content{}, label_node},
															   {Layout::Content{}, connect_button_node},
															   {Layout::Content{}, make_creature_button_node}}}});
			}

			void run() {
				update_components();
			};
	};

	class Synchronizer {
		private:


			WorldEnvironment environment;
			Viewer           viewer;

		public:

			Synchronizer(Panel &panel)
				: viewer(environment, panel) {}

			bool has_current_world() const {
				return environment.has_current_world();
			}

			bool is_connected() const {
				return environment.is_connected();
			}

			void connect() {
				environment.connect();
			}

			void disconnect() {
				environment.disconnect();
			}

			void run() {
				viewer.run();
			}
	};
} // namespace KApp
