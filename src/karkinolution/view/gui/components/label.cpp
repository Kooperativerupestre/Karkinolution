#include <karkinolution/view/gui/components/aliases.hpp>
#include <karkinolution/view/gui/components/label.hpp>

void KGComponents::Label::draw() const {
	DrawText(text_.data(), static_cast<int>(bounds_.x), static_cast<int>(bounds_.y), 20, BLACK);
}