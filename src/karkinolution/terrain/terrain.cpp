#include "karkinolution/terrain/terrain.hpp"

#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/terrain/rtree/box.hpp"
#include "karkinolution/terrain/soil.hpp"

Box3D BoxConversion::to_box(const SoilPiece &soil) {
	Vec3 offset{soil.radius.value, soil.radius.value, 0.0};
	return Box3D{soil.position + offset, soil.position - offset};
}

Box3D BoxConversion::to_box(const Size &size, const Vec3 &position) {

	Vec3 half_extents{size.lateral.value / 2, size.height.value / 2, size.back.value / 2};
	return Box3D{
		position + half_extents, // max
		position - half_extents  // min
	};
}

Box3D BoxConversion::to_box(const Radius &radius, const Vec3 &center) {
	Vec3 extent{radius.value, radius.value, radius.value};

	return Box3D{center - extent, center + extent};
}

bool Territory::add(SoilPiece &&piece) {
	const auto piece_box = BoxConversion::to_box(piece);

	if (!Box3DMotor::contains(box(), piece_box)) {
		return false;
	}

	SoilPieceId id  = piece.id;
	Box3D       box = BoxConversion::to_box(piece);

	if (soils_.exists(id) || data_.exists(id)) {
		return false;
	}

	bool added = soils_.try_add(id, std::move(piece));
	if (!added) {
		return false;
	}

	data_.insert(id, box);
	return true;
}

std::vector<SoilPieceId> Territory::find(const SoilPiece &piece) const {
	auto box = BoxConversion::to_box(piece);
	return data_.find(box);
}

std::vector<SoilPieceId> Territory::find(const Radius &radius, const Vec3 &position) const {
	auto box = BoxConversion::to_box(radius, position);
	return data_.find(box);
}

bool Territory::remove(SoilPieceId id) {
	if (!data_.exists(id) || !soils_.exists(id)) {
		return false;
	}

 	auto was_be_removed = data_.remove(id);
	if (!was_be_removed) {
		return false;
	}

	return soils_.try_del(id);
}
