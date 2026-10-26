#pragma once
#include "karkinolution/binary/protocol/aliases.hpp"

#include <karkinolution/binary/byte_range.hpp>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/frames/invariants.hpp>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Frames {
			namespace Models {
				struct Request {
						FrameDeclaredSize      size;
						RequestId              id;
						RequestSubTypes        sub_type;
						std::vector<std::byte> payload;
						Resource               resource;
				};

				struct ResponseGeneric {
						std::uint32_t                                 size;
						RequestId                                     request_id;
						std::variant<ErrorSubTypes, ResponseSubTypes> sub_type;
						std::vector<std::byte>                        payload;
						BinaryTypes                                   type;

						template <ByteRange Payload>
						ResponseGeneric(uint32_t                                      p_size,
										RequestId                                     p_request_id,
										std::variant<ErrorSubTypes, ResponseSubTypes> p_sub_type,
										const Payload                                &p_payload,
										BinaryTypes                                   p_type) {
							if (p_type != BinaryTypes::Response && p_type != BinaryTypes::Error) {
								throw FrameInvariantError(
									"Failed to create a parsed generic response with "
									"type different from response and error");
							}

							size       = p_size;
							request_id = p_request_id;
							sub_type   = p_sub_type;
							payload    = std::vector<std::byte>(p_payload.begin(), p_payload.end());
							type       = p_type;
						}
				};

				class GenericFrame {
					private:

						std::variant<Request, ResponseGeneric> data_;
						BinaryTypes                            type_;

					public:

						GenericFrame(std::variant<Request, ResponseGeneric> data, BinaryTypes type)
							: data_(data)
							, type_(type) {}

						BinaryTypes type() const noexcept {
							return type_;
						}

						const auto &data() const noexcept {
							return data_;
						}
				};

			} // namespace Models
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution
