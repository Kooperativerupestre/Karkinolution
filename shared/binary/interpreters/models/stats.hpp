#pragma once

namespace KShared {
	namespace Binary {
		namespace Interpreters {
			namespace Models {
				namespace Stats {
					template <typename T> struct StatValue {
							T value, min, max;
					};
				} // namespace Stats
			} // namespace Models
		} // namespace Interpreters
	} // namespace Binary
} // namespace KShared
