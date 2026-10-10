#pragma once


#include "karkinolution/core/error.hpp"

#include <cstdint>
#include <magic_enum/magic_enum.hpp>
#include <optional>
#include <unordered_set>
#include <variant>
#include <vector>

namespace Properties {


	namespace Capabilities {
		enum class Move : uint8_t {
			WALK  = 0,
			SWIMM = 1
		};
	} // namespace Capabilities

	// Each new type of property MUST BE ADDED ON THE END
	using AnyProperty = std::variant<Capabilities::Move>;

	static_assert(std::variant_size_v<AnyProperty> <= 256);

	struct Kind {
			uint8_t value;
	};

	struct Type {
			uint8_t value;
	};

	static_assert(sizeof(Type) == 1);
	static_assert(sizeof(Kind) == 1);

	template <typename T, typename V> struct IsAlternativeOf;

	template <typename T, typename... Ts>
	struct IsAlternativeOf<T, std::variant<Ts...>>
		: std::bool_constant<(std::is_same_v<T, Ts> || ...)> {};

	template <typename T>
	concept PropertyEnum = IsAlternativeOf<T, AnyProperty>::value;

	class GenericProperty {
		public:

			using Variant = AnyProperty;

			template <PropertyEnum E>
			constexpr GenericProperty(E value)
				: data_(value) {}

			static constexpr std::optional<GenericProperty> fromRaw(Kind kind, Type raw) {
				return fromRawImpl<0>(kind.value, raw.value);
			}

			Kind constexpr kind() const {
				return Kind(data_.index());
			}

			Type constexpr raw() const {
				return std::visit(
					[](auto e) {
						return Type(static_cast<uint8_t>(magic_enum::enum_integer(e)));
					},
					data_);
			}

			template <typename E> constexpr bool is() const {
				return std::holds_alternative<E>(data_);
			}

			template <typename E> constexpr std::optional<E> as() const {
				if (auto* p = std::get_if<E>(&data_)) {
					return *p;
				}
				return std::nullopt;
			}

			bool constexpr operator==(const GenericProperty &) const = default;

		private:


			template <std::size_t I>
			static constexpr std::optional<GenericProperty> fromRawImpl(std::size_t kind,
																		uint8_t     raw) {
				if constexpr (I == std::variant_size_v<Variant>) {
					return std::nullopt;
				} else {
					if (kind != I) {
						return fromRawImpl<I + 1>(kind, raw);
					}
					using E = std::variant_alternative_t<I, Variant>;
					if (auto e = magic_enum::enum_cast<E>(raw)) {
						return GenericProperty(*e);
					}
					return std::nullopt;
				}
			}

			Variant data_;
	};
} // namespace Properties

using Properties::GenericProperty;

template <> struct std::hash<Properties::GenericProperty> {
		std::size_t operator()(const Properties::GenericProperty &p) const noexcept {
			return (p.kind().value << 8) | p.raw().value;
		}
};

struct PropertyConflict {
		GenericProperty A;
		GenericProperty B;
};

inline constexpr PropertyConflict GLOBAL_PROPERTY_CONFLICTS[] = {};

class PropertiesContainer {
	public:

		std::unordered_set<GenericProperty> value;

		bool has(GenericProperty property) const;
		bool add(GenericProperty property);
		bool remove(GenericProperty property);
		void clear();
};
