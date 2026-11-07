#pragma once

#include "shared/core/error.hpp"

namespace KarkinolutionSDK {

	class CPPSdkError : public KShared::Errors::SimulationError {
		public:

			explicit CPPSdkError(const std::string &message)
				: KShared::Errors::SimulationError(message) {}
	};

	class ClientError : public CPPSdkError {
		public:

			explicit ClientError(const std::string &message)
				: CPPSdkError(message) {}
	};

} // namespace KarkinolutionSDK