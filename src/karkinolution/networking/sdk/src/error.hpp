#pragma once

#include "karkinolution/core/error.hpp"

namespace KarkinolutionSDK {

	class CPPSdkError : public Karkinolution::Errors::SimulationError {
		public:

			explicit CPPSdkError(const std::string &message)
				: Karkinolution::Errors::SimulationError(message) {}
	};

	class ClientError : public CPPSdkError {
		public:

			explicit ClientError(const std::string &message)
				: CPPSdkError(message) {}
	};

} // namespace KarkinolutionSDK