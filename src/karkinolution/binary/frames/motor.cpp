

#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/message_type_size.hpp>

std::vector<std::byte> FrameMotor::build(BinarySubTypes::Request       sub_type,
										 const std::vector<std::byte> &payload) {

	return _build_impl(sub_type, BinaryTypes::Request, payload);
}

std::vector<std::byte> FrameMotor::build(BinarySubTypes::Error         sub_type,
										 const std::vector<std::byte> &payload) {

	return _build_impl(sub_type, BinaryTypes::Error, payload);
}

std::vector<std::byte> FrameMotor::build(BinarySubTypes::Response      sub_type,
										 const std::vector<std::byte> &payload) {
	return _build_impl(sub_type, BinaryTypes::Error, payload);
}
