#pragma once

#include "karkinolution/networking/client_side/interpreters/concepts.hpp"

namespace KApp {
	namespace Commanders {
		using Karkinolution::Networking::ClientSide::AsClient;
		using Karkinolution::Networking::ClientSide::CanHandleConnection;
		using Karkinolution::Networking::ClientSide::CanRequest;
		using Karkinolution::Networking::ClientSide::CanRequestSystemScope;
		using Karkinolution::Networking::ClientSide::CanRequestWorldScope;
	} // namespace Commanders
} // namespace KApp