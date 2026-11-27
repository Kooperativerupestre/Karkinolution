#pragma once


#include "karkinolution/networking/client_side/client/client.hpp"
#include "karkinolution/networking/client_side/world/world.hpp"
#include "karkinolution/networking/client_side/world/worlds_manager.hpp"
#include "opsiamethystos/graphics/panel/components/component.hpp"
#include "opsiamethystos/graphics/panel/components/low_level/label.hpp"
#include "opsiamethystos/graphics/panel/components/storage.hpp"
#include "opsiamethystos/graphics/panel/panel.hpp"

#include <opsiamethystos/graphics/panel/components/low_level/button.hpp>
#include <opsiamethystos/math/geometry/geometry.hpp>

namespace KApp {

	using Client        = Karkinolution::Networking::ClientSide::Client::RuntimeClient;
	using World         = Karkinolution::Networking::ClientSide::World::World;
	using WorldsManager = Karkinolution::Networking::ClientSide::World::WorldsManager<Client>;
	namespace Math      = Opsiamethystos::Math;

	using Button = Opsiamethystos::Graphics::Panel::Components::LowLevel::Button::RectangleButton;
	using Opsiamethystos::Graphics::Panel::Components::LowLevel::Button::ButtonStyles;
	using Label     = Opsiamethystos::Graphics::Panel::Components::LowLevel::RectangleLabel;
	using Component = Opsiamethystos::Gui::Components::Component;
	using Opsiamethystos::Graphics::Panel::Components::ComponentsStorage;
	using Opsiamethystos::Gui::Panel::Panel;
	using WorldResponseResult  = Karkinolution::Networking::ClientSide::Client::WorldResponseResult;
	using WorldRequestData     = Karkinolution::Networking::ClientSide::Client::WorldRequestData;
	using SystemResponseResult = Karkinolution::Networking::ClientSide::Client::SystemResponseResult;
	using SystemRequestData    = Karkinolution::Networking::ClientSide::Client::SystemRequestData;
} // namespace KApp