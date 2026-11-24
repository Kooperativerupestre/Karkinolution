#include "karkinolution/networking/frame/system/core_aliases.hpp"
#include "karkinolution/networking/frame/system/models.hpp"

#include <karkinolution/networking/frame/system/motor.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::System {
			Models::SystemOutput::SystemOutput Motor::make_none_system_output(SystemCall::Id call_id) {
				return make_system_output(SystemOutputSubTypes::None, call_id, std::vector<std::byte>{});
			}
		} // namespace Frames::System
	} // namespace Networking
} // namespace Karkinolution