#pragma once
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/math/units.hpp"
#include "karkinolution/terrain/rtree/box.hpp"
#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/soil.hpp>

class SoilPieceRegistry : public BaseStorage<SoilPieceId, SoilPiece> {
  public:
    using BaseStorage<SoilPieceId, SoilPiece>::BaseStorage;
};

class Territory {
  private:
    Vec3 size_max_, size_min_;

    RStarTree<SoilPieceId> data_;
    SoilPieceRegistry soils_;

  public:
    const Vec3& size_max() const noexcept { return size_max_; }
    const Vec3& size_min() const noexcept { return size_min_; }

    const SoilPieceRegistry& soils() const { return soils_; }

    bool add(SoilPiece&& piece);
    bool remove(SoilPieceId id);
    std::vector<SoilPieceId> find(const SoilPiece& piece) const;
    std::vector<SoilPieceId> find(const Radius& radius, const Vec3& position) const;
};

namespace BoxConversion {
Box3D to_box(const SoilPiece& piece);
Box3D to_box(const Size& size, const Vec3& position);
Box3D to_box(const Radius& radius, const Vec3& position);
} // namespace BoxConversion
