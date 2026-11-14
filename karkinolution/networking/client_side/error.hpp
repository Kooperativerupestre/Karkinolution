#pragma once

#include "shared/core/error.hpp"

#include <string>

namespace Karkinolution::Networking::ClientSide {
	class ClientSideError : public KShared::Errors::SimulationError {
		public:

			explicit ClientSideError(const std::string &message)
				: KShared::Errors::SimulationError(message) {}
	};

	class ClientError : public ClientSideError {
		public:

			explicit ClientError(const std::string &message)
				: ClientSideError(message) {}
	};
} // namespace Karkinolution::Networking::ClientSide
