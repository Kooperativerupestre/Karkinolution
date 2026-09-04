#pragma once
#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/math/physic/vec/model.hpp"
#include "karkinolution/math/units.hpp"
#include "karkinolution/terrain/rtree/box.hpp"

#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/soil.hpp>

namespace BoxConversion {
	Box3D to_box(const SoilPiece &piece);
	Box3D to_box(const Size &size, const Vec3 &position);
	Box3D to_box(const Radius &radius, const Vec3 &position);
} // namespace BoxConversion

class SoilPieceRegistry : public BaseStorage<SoilPieceId, SoilPiece> {
	public:

		using BaseStorage<SoilPieceId, SoilPiece>::BaseStorage;
};

class Territory {
	private:

		Size size_;


		RStarTree<SoilPieceId> data_;
		SoilPieceRegistry      soils_;

	public:

		Territory(const Size &size)
			: size_(size) {}

		const Size &size() const {
			return size_;
		}

		const SoilPieceRegistry &soils() const {
			return soils_;
		}

		Box3D box() const {
			return BoxConversion::to_box(size_, Vec3{0, 0, 0});
		}

		bool                     add(SoilPiece &&piece);
		bool                     remove(SoilPieceId id);
		std::vector<SoilPieceId> find(const SoilPiece &piece) const;
		std::vector<SoilPieceId> find(const Radius &radius, const Vec3 &position) const;
};
