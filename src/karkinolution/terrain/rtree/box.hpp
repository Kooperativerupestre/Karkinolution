#pragma once
#include <shared/math/physic/vec/model.hpp>

namespace Karkinolution::Terrain::Trees::RTree {
	enum class Bound : uint8_t {
		MAX,
		MIN
	};

	struct Box2D {
			KShared::Math::Vec::Vec2 max, min;

			const KShared::Math::Vec::Vec2 &operator[](Bound) const noexcept;
			KShared::Math::Vec::Vec2 &operator[](Bound) noexcept;

			[[nodiscard]] double                         width() const noexcept;
			[[nodiscard]] double                         height() const noexcept;
			[[nodiscard]] double                         area() const noexcept;
			[[nodiscard]] double                         margin() const noexcept;
			[[nodiscard]] KShared::Math::Vec::Vec2 center() const noexcept;

			[[nodiscard]] Box2D operator+(const Box2D &other) const;
	};

	constexpr size_t index(KShared::Math::Vec::Axis axis) {
		return static_cast<size_t>(axis);
	}

	constexpr size_t index(Bound bound) {
		return static_cast<size_t>(bound);
	}

	namespace Box2DMotor {
		[[nodiscard]] bool   intersect(const Box2D &A, const Box2D &B);
		[[nodiscard]] double overlap_area(const Box2D &A, const Box2D &B);
		[[nodiscard]] Box2D  intersection(const Box2D &A, const Box2D &B);
		[[nodiscard]] Box2D  combine(const Box2D &A, const Box2D &B);
		[[nodiscard]] double enlargement(const Box2D &current, const Box2D &canditate);
		[[nodiscard]] bool   contains(const Box2D &container, const Box2D &contained);
	} // namespace Box2DMotor
} // namespace Karkinolution::Terrain::Trees::RTree
