#include "karkinolution/networking/frame/core_aliases.hpp"
#include "karkinolution/networking/frame/models.hpp"

#include <karkinolution/networking/frame/motor.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			Models::SystemOutput::SystemOutput Motor::make_none_system_output(SystemCall::Id call_id) {
				return make_system_output(SystemOutputSubTypes::None, call_id, std::vector<std::byte>{});
			}
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution