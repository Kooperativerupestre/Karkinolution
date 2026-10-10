#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/networking/protocol_session.hpp"
#include "karkinolution/networking/sdk/src/error.hpp"

#include <asio/connect.hpp>
#include <asio/write.hpp>
#include <chrono>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/networking/asio_types.hpp>
#include <karkinolution/networking/sdk/src/client/client.hpp>
#include <thread>
using KarkinolutionSDK::Client::Client;

void Client::on_error(const std::string &error_message) {
	std::cerr << "Error: " << error_message << "\n";

	disconnect();
}

void Client::connect() {
	if (connected) {
		throw ConnectionError("Client is already connected");
	}

	AsioResolver resolver(io_context_);

	AsioErrorCode error;

	const auto endpoints = resolver.resolve(host_, std::to_string(port), error);

	if (error) {
		throw ConnectionError(
			std::format("Failed to resolve {}:{}: {}", host_, port, error.message()));
	}

	error.clear();

	asio::connect(socket_, endpoints, error);

	if (error) {
		throw ConnectionError(
			std::format("Failed to connect to {}:{}: {}", host_, port, error.message()));
	}
	connected = true;
}

RequestId Client::send_without_waiting(RequestSubTypes            sub_type,
									   RequestResource            resource,
									   std::span<const std::byte> payload) {
	KarkinolutionSDK::Client::Validator::validate_is_connected(*this);
	const auto request_id = next_request_id.fetch_add(1);
	const auto raw_frame =
		Frames::Motor::make_request_raw_bytes(sub_type, request_id, resource, payload);

	protocol_session.add_request(raw_frame);
	send();
	return request_id;
}

void Client::send() {
	KarkinolutionSDK::Client::Validator::validate_is_connected(*this);
	if (sending_state.is_sending) {
		return;
	}

	auto bytes = protocol_session.pop_request();

	if (!bytes) {
		return;
	}

	sending_state.is_sending = true;
	sending_state.bytes      = bytes.value();

	asio::async_write(socket_,
					  asio::buffer(sending_state.bytes.data(), sending_state.bytes.size()),

					  [this](const AsioErrorCode &error, std::size_t) {
						  if (error) {
							  on_error(error.message());
							  return;
						  }
						  clear_sending_state();

						  std::cout << "Request sent 🦀\n";
						  send();
					  });
}

void Client::receive() {
	socket_.async_read_some(
		asio::buffer(temporary_responses),
		[this](const AsioErrorCode &error, std::size_t bytes_received) {
			if (error) {
				if (error == asio::error::eof) {
					on_closed();
					return;
				}

				on_error(error.message());
				return;
			}

			protocol_session.receive(
				std::span<const std::byte>{temporary_responses.data(), bytes_received});

			send();
			receive();
		});
}

std::optional<ResponseFrame> Client::pop_response(RequestId id) {
	return protocol_session.pop_response(id);
}

std::optional<ResponseFrame> Client::request(RequestSubTypes            sub_type,
											 RequestResource            resource,
											 std::span<const std::byte> payload) {


	const auto                   id       = send_without_waiting(sub_type, resource, payload);
	std::optional<ResponseFrame> response = protocol_session.await_response(id);
	return response;
}