#include <imgui.h>
#include <karkinolution/view/gui/components/button.hpp>

namespace {
	namespace KC = KarkinolutionView::Gui::Components;

	ImVec4 to_imgui(const Karkinolution::Colors::Color &color) {
		return ImVec4{color.r.value.value(),
					  color.g.value.value(),
					  color.b.value.value(),
					  color.a.value.value()};
	}

	// Pushes the style on construction and pops it on destruction, so it can never leak
	// into the next widget.
	class ScopedButtonStyle {
		public:

			explicit ScopedButtonStyle(const KC::ButtonStyle &style) {
				ImGui::PushStyleColor(ImGuiCol_Button, to_imgui(style.idle));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, to_imgui(style.hovered));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, to_imgui(style.pressed));
				ImGui::PushStyleColor(ImGuiCol_Text, to_imgui(style.text));
			}

			~ScopedButtonStyle() {
				ImGui::PopStyleColor(pushed_colors);
			}

			ScopedButtonStyle(const ScopedButtonStyle &)            = delete;
			ScopedButtonStyle &operator=(const ScopedButtonStyle &) = delete;

		private:

			static constexpr int pushed_colors = 4;
	};
} // namespace

bool KarkinolutionView::Gui::Components::Button::draw() const {
	const ScopedButtonStyle scoped_style{style};

	// A zero axis tells ImGui to size that axis from the label.
	const ImVec2 size{width ? static_cast<float>(width->value()) : 0.0f,
					  height ? static_cast<float>(height->value()) : 0.0f};

	return ImGui::Button(label.c_str(), size);
}