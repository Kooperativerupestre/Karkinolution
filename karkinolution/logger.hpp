#pragma once

#include "shared/kloggin/levels.hpp"
#include "shared/kloggin/logger.hpp"
#include "shared/kloggin/sink/console_sink.hpp"

#include <utility>

namespace Karkinolution {
	inline KShared::KLoggin::Sink::ConsoleSink<KShared::KLoggin::Sink::ConsoleSinkOutput::STDOUT> _console_;
	inline KShared::KLoggin::Logger logger{std::move(_console_)};
	using LogLevel = KShared::KLoggin::Level;

} // namespace Karkinolution