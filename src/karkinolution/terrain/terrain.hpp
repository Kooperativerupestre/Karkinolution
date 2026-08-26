#pragma once
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/math/units.hpp"
#include "karkinolution/terrain/rtree/box.hpp"
#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/soil.hpp>

using Territory = RStarTree;
class SoilPieceRegistry : public BaseStorage<SoilPieceId, SoilPiece> {
  public:
    using BaseStorage<SoilPieceId, SoilPiece>::BaseStorage;
};

namespace BoxConversion {
Box3D to_box(const SoilPiece& piece);
Box3D to_box(const Size& size, const Vec3& position);
} // namespace BoxConversion

namespace TerritoryMotor {
bool add(SoilPiece&& piece, Territory& territory, SoilPieceRegistry& registry);
bool remove(SoilPieceId id, Territory& territory, SoilPieceRegistry& registry);
std::vector<SoilPieceId> find(const SoilPiece& piece, const Territory& territory);
std::vector<SoilPieceId> find(const Size& size, const Vec3& position, const Territory& territory);
} // namespace TerritoryMotor