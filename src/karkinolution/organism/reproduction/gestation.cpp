#include <karkinolution/organism/reproduction/gestation.hpp>

float Gestation::progress() const {
	return static_cast<float>(value()) * value() / (static_cast<float>(max()) * max());
}

bool Gestation::is_ready_to_born() const {
	return value() >= max();
}

uint16_t Gestation::overdue() const {
	return std::max<uint16_t>(0, (value() - max()) ? value() > max() : 0);
}
