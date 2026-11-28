#pragma once

#include "shared/kloggin/levels.hpp"
#include "shared/kloggin/logger.hpp"
#include "shared/kloggin/sink/console_sink.hpp"

namespace KApp {
	KShared::KLoggin::Sink::ConsoleSink<KShared::KLoggin::Sink::ConsoleSinkOutput::STDOUT> _console{};

	KShared::KLoggin::Logger app_logger(std::move(_console));
	using LogLevel = KShared::KLoggin::Level;

} // namespace KApp