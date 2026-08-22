#include <algorithm>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/terrain/rtree/box.hpp>
#include <utility>

const Vec3& Box3D::operator[](Bound bound) const noexcept {
    if (bound == Bound::MAX) {
        return max;
    } else if (bound == Bound::MIN) {
        return min;
    }

    std::unreachable();
}

Vec3& Box3D::operator[](Bound bound) noexcept {
    if (bound == Bound::MAX) {
        return max;
    } else if (bound == Bound::MIN) {
        return min;
    }

    std::unreachable();
}

[[nodiscard]] double Box3D::width() const noexcept { return max.x - min.x; }

[[nodiscard]] double Box3D::height() const noexcept { return max.y - min.y; }

[[nodiscard]] double Box3D::depth() const noexcept { return max.z - min.z; }

[[nodiscard]] double Box3D::volume() const noexcept { return width() * height() * depth(); }

[[nodiscard]] double Box3D::margin() const noexcept { return 2.0 * (width() + depth() + height()); }

[[nodiscard]] Box3D Box3D::operator+(const Box3D& other) const {
    return Box3D{.max = max + other.max, .min = min + other.min};
}

[[nodiscard]] bool Box3DMotor::intersect(const Box3D& A, const Box3D& B) {
    return A.min.x <= B.max.x && A.max.x >= B.min.x &&

           A.min.y <= B.max.y && A.max.y >= B.min.y &&

           A.min.z <= B.max.z && A.max.z >= B.min.z;
}

[[nodiscard]] Box3D Box3DMotor::intersection(const Box3D& A, const Box3D& B) {
    return Box3D{.max = Vec3{std::min(A.max.x, B.max.x), std::min(A.max.y, B.max.y),
                             std::min(A.max.z, B.max.z)},
                 .min = Vec3{std::max(A.min.x, B.min.x), std::max(A.min.y, B.min.y),
                             std::max(A.min.z, B.min.z)}};
}

[[nodiscard]] double Box3DMotor::overlap_volume(const Box3D& A, const Box3D& B) {
    const double dx = std::min(A.max.x, B.max.x) - std::max(A.min.x, B.min.x);
    const double dy = std::min(A.max.y, B.max.y) - std::max(A.min.y, B.min.y);
    const double dz = std::min(A.max.z, B.max.z) - std::max(A.min.z, B.min.z);

    if (dx <= 0.0 || dy <= 0.0 || dz <= 0.0) {
        return 0.0;
    }

    return dx * dy * dz;
}

[[nodiscard]] Vec3 Box3D::center() const noexcept {
    return Vec3{(min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2};
}

[[nodiscard]] Box3D Box3DMotor::combine(const Box3D& A, const Box3D& B) {
    return Box3D{.max = Vec3{std::max(A.max.x, B.max.x), std::max(A.max.y, B.max.y),
                             std::max(A.max.z, B.max.z)},
                 .min = Vec3{std::min(A.min.x, B.min.x), std::min(A.min.y, B.min.y),
                             std::min(A.min.z, B.min.z)}};
}

[[nodiscard]] double Box3DMotor::enlargement(const Box3D& current, const Box3D& canditate) {
    return combine(current, canditate).volume() - current.volume();
}