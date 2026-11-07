#pragma once
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::Organisms::Entities::Embryo::Motor {
	namespace EmbryoMotor {
		void grow(Embryo &embryo);

		void run(Embryo &embryo, Karkinolution::World::World &world);
	} // namespace EmbryoMotor
} // namespace Karkinolution::Organisms::Entities::Embryo::Motor
