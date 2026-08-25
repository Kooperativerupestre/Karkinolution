#pragma once
#include <karkinolution/math/physic/vec/model.hpp>

enum class Bound : uint8_t { MAX, MIN };

struct Box3D {
  Vec3 max{}, min{};

  const Vec3 &operator[](Bound) const noexcept;
  Vec3 &operator[](Bound) noexcept;

  [[nodiscard]] double width() const noexcept;
  [[nodiscard]] double height() const noexcept;
  [[nodiscard]] double depth() const noexcept;
  [[nodiscard]] double volume() const noexcept;
  [[nodiscard]] double margin() const noexcept;
  [[nodiscard]] Vec3 center() const noexcept;

  [[nodiscard]] Box3D operator+(const Box3D &other) const;
};

constexpr size_t index(Axis axis) { return static_cast<size_t>(axis); }
constexpr size_t index(Bound bound) { return static_cast<size_t>(bound); }

namespace Box3DMotor {
[[nodiscard]] bool intersect(const Box3D &A, const Box3D &B);
[[nodiscard]] double overlap_volume(const Box3D &A, const Box3D &B);
[[nodiscard]] Box3D intersection(const Box3D &A, const Box3D &B);
[[nodiscard]] Box3D combine(const Box3D &A, const Box3D &B);
[[nodiscard]] double enlargement(const Box3D &current, const Box3D &canditate);
} // namespace Box3DMotor