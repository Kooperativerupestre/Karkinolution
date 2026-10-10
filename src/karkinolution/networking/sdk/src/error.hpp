#pragma once

#include "karkinolution/core/error.hpp"

class CPPSdkError : public SimulationError {
	public:

		explicit CPPSdkError(const std::string &message)
			: SimulationError(message) {}
};

class ClientError : public CPPSdkError {
	public:

		explicit ClientError(const std::string &message)
			: CPPSdkError(message) {}
};