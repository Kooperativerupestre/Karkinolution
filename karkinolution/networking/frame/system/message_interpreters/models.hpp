#pragma once

namespace Karkinolution::Networking::Frames::System::MessageInterpreters::Models {
	// CREATE_WORLD returns protocol state rather than a simulation object.
	struct WorldCreatedOutput {
			bool world_created;
	};
} // namespace Karkinolution::Networking::Frames::System::MessageInterpreters::Models
