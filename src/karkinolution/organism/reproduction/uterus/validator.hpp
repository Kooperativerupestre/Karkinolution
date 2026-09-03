#pragma once
#include <cassert>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>

namespace UterusValidator {
inline void is_pregnant(const Uterus &uterus) {
	return assert(uterus.is_pregnant());
}

inline void is_empty(const Uterus &uterus) {
	return assert(uterus.is_empty());
}
} // namespace UterusValidator