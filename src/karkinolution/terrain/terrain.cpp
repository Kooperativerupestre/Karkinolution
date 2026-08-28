#include "karkinolution/terrain/terrain.hpp"
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/terrain/rtree/box.hpp"
#include "karkinolution/terrain/soil.hpp"

Box3D BoxConversion::to_box(const SoilPiece& soil) {
    Vec3 offset{soil.radius.value, soil.radius.value, 0.0};
    return Box3D{soil.position + offset, soil.position - offset};
}

Box3D BoxConversion::to_box(const Size& size, const Vec3& position) {

    Vec3 half_extents{size.lateral.value / 2, size.height.value / 2, size.back.value / 2};
    return Box3D{
        position + half_extents, // max
        position - half_extents  // min
    };
}

Box3D BoxConversion::to_box(const Radius& radius, const Vec3& center) {
    Vec3 extent{radius.value, radius.value, radius.value};

    return Box3D{center - extent, center + extent};
}

bool TerritoryMotor::add(SoilPiece&& piece, Territory& territory, SoilPieceRegistry& registry) {
    SoilPieceId id = piece.id;
    Box3D box = BoxConversion::to_box(piece);

    bool added = registry.try_add(id, std::move(piece));
    if (!added) {
        return false;
    }

    territory.insert(id, box);
    return true;
}

std::vector<SoilPieceId> TerritoryMotor::find(const SoilPiece& piece, const Territory& territory) {
    auto box = BoxConversion::to_box(piece);
    return territory.find(box);
}

std::vector<SoilPieceId> TerritoryMotor::find(const Size& size, const Vec3& position,
                                              const Territory& territory) {
    auto box = BoxConversion::to_box(size, position);
    return territory.find(box);
}

std::vector<SoilPieceId> TerritoryMotor::find(const Radius& radius, const Vec3& position,
                                              const Territory& territory) {
    auto box = BoxConversion::to_box(radius, position);
    return territory.find(box);
}
bool TerritoryMotor::remove(SoilPieceId id, Territory& territory, SoilPieceRegistry& registry) {
    auto was_be_removed = territory.delete_soil_piece(id);
    if (!was_be_removed) {
        return false;
    }
    registry.del(id);
    return true;
}
