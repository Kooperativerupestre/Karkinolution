#pragma once
#include <concepts>

namespace Karkinolution {
	namespace Math {
		namespace LV {

			template <typename T>
			concept LimitedValueLike = requires(const T value) {
				typename T::value_type;

				{ value.value() } -> std::same_as<typename T::value_type>;
				{ value.max } -> std::same_as<const typename T::value_type &>;
				{ value.min } -> std::same_as<const typename T::value_type &>;
			};

			template <typename T>
			concept RuntimeLimitedValueLike = requires(T value) {
				typename T::value_type;

				{ value.max() } -> std::same_as<typename T::value_type>;
				{ value.min() } -> std::same_as<typename T::value_type>;
				{ value.value() } -> std::same_as<typename T::value_type>;
			};

			template <typename T>
			concept StatLike = LimitedValueLike<T> || RuntimeLimitedValueLike<T>;

			template <typename T> struct GenericStat {
					T value, min, max;
			};

			template <StatLike T> auto get(const T &stat) {
				using Type = typename T::value_type;
				GenericStat<Type> result;

				result.value = stat.value();

				if constexpr (LimitedValueLike<T>) {
					result.max = stat.max;
					result.min = stat.min;
				} else {
					result.max = stat.max();
					result.min = stat.min();
				}

				return result;
			}

		} // namespace LV
	} // namespace Math
} // namespace Karkinolution
