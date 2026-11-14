
#include <karkinolution/view/screen.hpp>
#include <stdexcept>

KarkinolutionView::Screen::StateView::StateView(const State &state)
	: state_(state) {}

KarkinolutionView::Screen::StateView::StateView(const State &state, Opsiamethystos::Gui::Panel::Panel &panel)
	: state_(state) {
	register_panel(panel);
}

void KarkinolutionView::Screen::StateView::register_panel(Opsiamethystos::Gui::Panel::Panel &panel) {
	if (component_) {
		return;
	}

	component_ =
		std::make_shared<Opsiamethystos::Gui::Components::Component>(Opsiamethystos::Gui::Components::AllComponents{
			Opsiamethystos::Gui::Components::LowLevel::Text{Gui::Geometry::Rectangle{Gui::Geometry::Position{-290, 95},
																					 Gui::Geometry::Width{140},
																					 Gui::Geometry::Height{40}},
															state_.connected ? "Connected" : "Disconnected",
															KShared::Colors::Presets::DarkGray,
															20}});

	if (!panel.add(component_)) {
		throw std::runtime_error("Failed to add connection status text to panel");
	}
}

void KarkinolutionView::Screen::StateView::sync() {
	const std::string target = state_.connected ? "Connected" : "Disconnected";
	if (component_) {
		auto &text = std::get<Opsiamethystos::Gui::Components::LowLevel::Text>(component_->get_data());
		if (text.get_text() != target) {
			text.set_text(target);
		}
	} else {
		if (fallback_status_.get_text() != target) {
			fallback_status_.set_text(target);
		}
	}
}

void KarkinolutionView::Screen::StateView::draw() {
	sync();
	if (!component_) {
		fallback_status_.draw();
	}
}