#pragma once


#include <array>
#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <karkinolution/binary/serialization/serializer.hpp>
#include <vector>

namespace FrameMotor {
	// don't use that out of here
	template <typename SubType, typename Payload>
	std::vector<std::byte> _build_impl(SubType sub_type, BinaryTypes type, const Payload &payload) {

		std::vector<std::byte> frame;

		const std::uint32_t size = MESSAGE_SUB_TYPE_BYTES + MESSAGE_TYPE_BYTES + payload.size();

		frame.reserve(size + MESSAGE_SIZE_BYTES);

		Deserializer::append_bytes(frame, Serializer::convert_uint32_t(size));

		Deserializer::append_bytes(frame,
								   Serializer::convert_uint8_t(static_cast<std::uint8_t>(type)));

		Deserializer::append_bytes(
			frame,
			Serializer::convert_uint8_t(static_cast<std::uint8_t>(sub_type)));

		frame.insert(frame.end(), payload.begin(), payload.end());

		return frame;
	}

	template <typename SubType, typename Payload>
	std::vector<std::byte> _build_impl(SubType         sub_type,
									   BinaryTypes     type,
									   RequestResource resource,
									   const Payload  &payload) {
		std::vector<std::byte> frame;
		const std::uint32_t    size =
			MESSAGE_SUB_TYPE_BYTES + MESSAGE_TYPE_BYTES + payload.size() + REQUEST_RESOURCE_BYTES;

		frame.reserve(size + MESSAGE_SIZE_BYTES);
		Deserializer::append_bytes(frame, Serializer::convert_uint32_t(size));
		Deserializer::append_bytes(frame,
								   Serializer::convert_uint8_t(static_cast<std::uint8_t>(type)));
		Deserializer::append_bytes(
			frame,
			Serializer::convert_uint8_t(static_cast<std::uint8_t>(sub_type)));

		Deserializer::append_bytes(frame,
								   static_cast<std::byte>(static_cast<std::uint8_t>(resource)));
		frame.insert(frame.end(), payload.begin(), payload.end());
		return frame;
	}

	std::uint32_t                get_size(const std::vector<std::byte> &bytes);
	BinaryTypes                  get_code(const std::vector<std::byte> &bytes);
	BinarySubTypes::CodeSubTypes get_type(const std::vector<std::byte> &bytes);


	std::vector<std::byte> get_payload(const std::vector<std::byte> &bytes);

	template <size_t SIZE>
	std::vector<std::byte> build(BinarySubTypes::Request            sub_type,
								 RequestResource                    resource,
								 const std::array<std::byte, SIZE> &payload) {

		return _build_impl(sub_type, BinaryTypes::Request, resource, payload);
	}

	template <size_t SIZE>
	std::vector<std::byte> build(BinarySubTypes::Error              sub_type,
								 const std::array<std::byte, SIZE> &payload) {

		return _build_impl(sub_type, BinaryTypes::Error, payload);
	}

	template <size_t SIZE>
	std::vector<std::byte> build(BinarySubTypes::Response           sub_type,
								 const std::array<std::byte, SIZE> &payload) {

		return _build_impl(sub_type, BinaryTypes::Response, payload);
	}

	std::vector<std::byte> build(BinarySubTypes::Request, const std::vector<std::byte> &payload);
	std::vector<std::byte> build(BinarySubTypes::Error, const std::vector<std::byte> &payload);
	std::vector<std::byte> build(BinarySubTypes::Response, const std::vector<std::byte> &payload);
} // namespace FrameMotor
