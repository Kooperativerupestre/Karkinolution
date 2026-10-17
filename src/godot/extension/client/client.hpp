#pragma once

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/networking/sdk/src/client/client.hpp"

#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <expected>
#include <thread>

namespace KarkinolutionGodot {
	namespace Client {
		class GodotClient : public godot::RefCounted {
				GDCLASS(GodotClient, godot::RefCounted);

			private:

				asio::io_context                                           io_context_;
				asio::executor_work_guard<asio::io_context::executor_type> work_guard_;
				std::thread                                                io_thread_;
				KarkinolutionSDK::Client::Client                           client;

			protected:

				static void _bind_methods();

			public:

				GodotClient();
				~GodotClient() override;

				// Connect and disconnect return godot::Error so GDScript receives native error
				// codes instead of uncaught C++ exceptions.
				godot::Error connect();
				godot::Error disconnect();
				bool         is_connected() const;

				// Internal C++ method (not exposed to ClassDB): returns std::expected to allow
				// higher-level interpreters to inspect the exact godot::Error or retrieve the
				// ResponseFrame.
				std::expected<ResponseFrame, godot::Error>
				request(const KarkinolutionSDK::Client::ClientRequestData &data);
		};
	} // namespace Client
} // namespace KarkinolutionGodot