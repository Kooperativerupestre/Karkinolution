#include "karkinolution/networking/sdk/src/client/client.hpp"

#include "godot/extension/model/aliases.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/networking/sdk/src/error.hpp"

#include <godot/extension/client/alias.hpp>
#include <string_view>

void GodotClient::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("connect"), &GodotClient::connect);
	godot::ClassDB::bind_method(godot::D_METHOD("disconnect"), &GodotClient::disconnect);
	godot::ClassDB::bind_method(godot::D_METHOD("is_connected"), &GodotClient::is_connected);

	// Decided to provide error_occurred signal so higher-level interpreters and callers can
	// asynchronously notify GDScript of any network or protocol errors without swallowing failures.
	ADD_SIGNAL(godot::MethodInfo("error_occurred",
								 godot::PropertyInfo(godot::Variant::INT, "error_code"),
								 godot::PropertyInfo(godot::Variant::STRING, "message")));
}

GodotClient::GodotClient()
	: io_context_()
	, work_guard_(asio::make_work_guard(io_context_))
	, io_thread_([this] {
		io_context_.run();
	})
	, client(io_context_) {}

GodotClient::~GodotClient() {
	client.disconnect();
	work_guard_.reset();
	io_context_.stop();
	if (io_thread_.joinable()) {
		io_thread_.join();
	}
}

godot::Error GodotClient::connect() {
	try {
		client.connect();
		return godot::OK;
	} catch (const ConnectionError &e) {
		return godot::ERR_CONNECTION_ERROR;
	} catch (const std::exception &e) {
		return godot::FAILED;
	} catch (...) {
		return godot::FAILED;
	}
}

godot::Error GodotClient::disconnect() {
	try {
		client.disconnect();
		return godot::OK;
	} catch (const std::exception &e) {
		return godot::FAILED;
	} catch (...) {
		return godot::FAILED;
	}
}

bool GodotClient::is_connected() const {
	return client.is_connected();
}

// Exception mapping design decision:
// - ConnectionError / client disconnection -> ERR_CONNECTION_ERROR
// - Corrupt or malformed frame / subtype invariants -> ERR_INVALID_DATA
// - Pending request duplicate -> ERR_ALREADY_EXISTS
// - Concurrent wait (std::logic_error) -> ERR_BUSY
// - SDK internal errors (CPPSdkError) / unexpected -> FAILED
// - Cancellation / wait timeout -> ERR_TIMEOUT
std::expected<ResponseFrame, godot::Error>
GodotClient::request(const KarkinolutionSDK::Client::ClientRequestData &data) {
	try {
		const auto result = client.request(data);
		if (!result) {
			return std::unexpected(godot::ERR_TIMEOUT);
		}
		if (!result.value().has_value()) {
			return std::unexpected(godot::ERR_INVALID_DATA);
		}
		return result.value().value();
	} catch (const ConnectionError &e) {
		return std::unexpected(godot::ERR_CONNECTION_ERROR);
	} catch (const ClientError &e) {
		const std::string_view msg = e.what();
		if (msg.find("disconnected") != std::string_view::npos) {
			return std::unexpected(godot::ERR_CONNECTION_ERROR);
		}
		if (msg.find("Response frame cannot") != std::string_view::npos) {
			return std::unexpected(godot::ERR_INVALID_DATA);
		}
		if (msg.find("already pending") != std::string_view::npos) {
			return std::unexpected(godot::ERR_ALREADY_EXISTS);
		}
		return std::unexpected(godot::FAILED);
	} catch (const CPPSdkError &e) {
		return std::unexpected(godot::FAILED);
	} catch (const std::logic_error &e) {
		return std::unexpected(godot::ERR_BUSY);
	} catch (const std::exception &e) {
		return std::unexpected(godot::FAILED);
	} catch (...) {
		return std::unexpected(godot::FAILED);
	}
}