
#include "raylib.h"

#include <karkinolution/view/gui/components/aliases.hpp>
#include <karkinolution/view/gui/components/counter.hpp>

KGComponents::Counter::Counter(const int &value, Vector2 position)
	: value_(value)
	, position_(position) {}

void KGComponents::Counter::draw() const {

	constexpr int font_size = 20;
	constexpr int padding   = 10;

	const char* text        = TextFormat("%d", value_);
	const int   text_width  = MeasureText(text, font_size);
	const float square_size = static_cast<float>(text_width + padding * 2);

	DrawRectangle(static_cast<int>(position_.x),
				  static_cast<int>(position_.y),
				  static_cast<int>(square_size),
				  static_cast<int>(square_size),
				  BLUE);

	DrawText(text,
			 static_cast<int>(position_.x + (square_size - text_width) / 2),
			 static_cast<int>(position_.y + (square_size - font_size) / 2),
			 font_size,
			 WHITE);
}