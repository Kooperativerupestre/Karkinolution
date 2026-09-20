#pragma once
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/world/world.hpp>

namespace EmbryoMotor {
	void grow(Embryo &embryo);

	void run(Embryo &embryo, World &world);
} // namespace EmbryoMotor
