#pragma once
#include "karkinolution/core/error.hpp"

#include <concepts>
#include <format>
#include <functional>
#include <karkinolution/binary/byte_utils.hpp>
#include <karkinolution/binary/deserializer.hpp>
#include <karkinolution/binary/serializer.hpp>
#include <karkinolution/binary/types/function_concepts.hpp>
#include <ranges>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Types {


			namespace List {

				using ListSize = std::uint16_t;

				template <typename R>
				concept ListContainer = std::ranges::sized_range<R>
					&& requires(R &&range, std::size_t index) { range[index]; };

				template <typename R>
				concept ListRange = std::ranges::input_range<R>;

				namespace Layout {

					using Size     = Field<0, sizeof(ListSize)>;
					using Elements = Next<Size, 0>;

					// The size of Elements is variable.
					// Do not use TOTAL_SIZE or END for this field.

				} // namespace Layout

				// The function already includes Elements::OFFSET.
				template <std::size_t Sizeof>
				constexpr std::size_t get_offset_based_on_index(std::size_t index,
																std::size_t offset) {

					return index * Sizeof + offset + Layout::Elements::OFFSET;
				}

				namespace SRI {

					namespace Details {

						template <typename R>
						concept SerializedByteRange =
							std::ranges::input_range<R> && std::ranges::sized_range<R>
							&& std::same_as<std::ranges::range_value_t<R>, std::byte>;

						template <typename R>
						concept IndexableSizedRange = ListContainer<R>;

						template <std::size_t SizeofSerializedObject>
						void validate_size(std::size_t size) {

							if (size > std::numeric_limits<ListSize>::max()) {
								throw ByteError("The number of elements exceeds the maximum "
												"representable by ListSize (uint16_t).");
							}

							constexpr auto max = std::numeric_limits<std::size_t>::max();

							if (size > (max - sizeof(ListSize)) / SizeofSerializedObject) {

								throw ByteError(
									"The serialized list size exceeds std::size_t limits.");
							}
						}

						template <typename Size> std::size_t checked_list_size(Size size) {

							if (size > std::numeric_limits<ListSize>::max()) {
								throw ByteError("The number of elements exceeds the maximum "
												"representable by ListSize (uint16_t).");
							}

							return static_cast<std::size_t>(size);
						}

						template <std::size_t SizeofSerializedObject>
						void ensure_can_append(std::size_t current_size) {

							if (current_size >= std::numeric_limits<ListSize>::max()) {

								throw ByteError("The number of elements exceeds the maximum "
												"representable by ListSize (uint16_t).");
							}

							constexpr auto max = std::numeric_limits<std::size_t>::max();

							constexpr auto max_elements =
								(max - sizeof(ListSize)) / SizeofSerializedObject;

							if (current_size >= max_elements) {
								throw ByteError(
									"The serialized list size exceeds std::size_t limits.");
							}
						}

						inline auto make_header(std::size_t size) {

							return Serializer::serialize<ListSize>(static_cast<ListSize>(size));
						}

						template <std::size_t SizeofSerializedObject>
						std::vector<std::byte> make_buffer(std::size_t size) {

							validate_size<SizeofSerializedObject>(size);

							const auto total_size =
								sizeof(ListSize) + SizeofSerializedObject * size;

							std::vector<std::byte> bytes;

							if (total_size > bytes.max_size()) {
								throw ByteError("The serialized list exceeds the maximum "
												"size supported by std::vector<std::byte>.");
							}

							bytes.reserve(total_size);

							const auto header = make_header(size);

							if (std::ranges::size(header) != sizeof(ListSize)) {
								throw ByteError(
									"The serialized list header has an unexpected size.");
							}

							bytes.insert(bytes.end(),
										 std::ranges::begin(header),
										 std::ranges::end(header));

							return bytes;
						}

						template <std::size_t SizeofSerializedObject, typename Serialized>
							requires SerializedByteRange<std::remove_cvref_t<Serialized>>
						void append_serialized(std::vector<std::byte> &bytes,
											   Serialized            &&serialized,
											   std::size_t             expected_offset) {

							const auto serialized_size = std::ranges::size(serialized);

							if (serialized_size != SizeofSerializedObject) {
								throw ByteError("The serialized object has a different size "
												"from SizeofSerializedObject.");
							}

							if (expected_offset != bytes.size()) {
								throw ByteError("The serialized element offset does not match "
												"the end of the output buffer.");
							}

							bytes.insert(bytes.end(),
										 std::ranges::begin(serialized),
										 std::ranges::end(serialized));
						}

						inline void write_header(std::vector<std::byte> &bytes, std::size_t size) {

							const auto header = make_header(size);

							if (std::ranges::size(header) != sizeof(ListSize)) {
								throw ByteError(
									"The serialized list header has an unexpected size.");
							}

							const auto header_offset = Layout::Size::OFFSET;

							if (header_offset > bytes.size()
								|| bytes.size() - header_offset < sizeof(ListSize)) {

								throw ByteError("The output buffer does not contain enough space "
												"for the list header.");
							}

							std::copy(std::ranges::begin(header),
									  std::ranges::end(header),
									  bytes.begin() + header_offset);
						}

						template <std::size_t SizeofSerializedObject,
								  typename R,
								  typename SerializerFunction>
						std::vector<std::byte> serialize_known_size(R                 &&range,
																	SerializerFunction &serializer,
																	std::size_t         size) {

							auto bytes = make_buffer<SizeofSerializedObject>(size);

							std::size_t index = 0;

							auto append_element = [&](auto &&element) {
								if (index >= size) {
									throw ByteError("The range produced more elements than "
													"its reported size.");
								}

								decltype(auto) serialized =
									std::invoke(serializer,
												std::forward<decltype(element)>(element));

								append_serialized<SizeofSerializedObject>(
									bytes,
									std::forward<decltype(serialized)>(serialized),
									get_offset_based_on_index<SizeofSerializedObject>(index, 0));

								++index;
							};

							if constexpr (IndexableSizedRange<R>) {

								for (; index < size;) {
									append_element(range[index]);
								}

							} else {

								for (auto &&element : range) {
									append_element(std::forward<decltype(element)>(element));
								}
							}

							if (index != size) {
								throw ByteError("The range produced fewer elements than "
												"its reported size.");
							}

							return bytes;
						}

						template <std::size_t SizeofSerializedObject, typename R>
						std::size_t count_forward_range(R &&range) {

							std::size_t size = 0;

							auto       iterator = std::ranges::begin(range);
							const auto sentinel = std::ranges::end(range);

							while (iterator != sentinel) {

								ensure_can_append<SizeofSerializedObject>(size);

								++size;
								++iterator;
							}

							return size;
						}

						template <std::size_t SizeofSerializedObject,
								  typename R,
								  typename SerializerFunction>
						std::vector<std::byte>
						serialize_single_pass(R &&range, SerializerFunction &serializer) {

							std::vector<std::byte> bytes(sizeof(ListSize), std::byte{});

							std::size_t size = 0;

							for (auto &&element : range) {

								ensure_can_append<SizeofSerializedObject>(size);

								decltype(auto) serialized =
									std::invoke(serializer,
												std::forward<decltype(element)>(element));

								append_serialized<SizeofSerializedObject>(
									bytes,
									std::forward<decltype(serialized)>(serialized),
									get_offset_based_on_index<SizeofSerializedObject>(size, 0));

								++size;
							}

							validate_size<SizeofSerializedObject>(size);

							write_header(bytes, size);

							return bytes;
						}

					} // namespace Details

					template <std::size_t SizeofSerializedObject,
							  typename R,
							  typename SerializerFunction>
					std::vector<std::byte> serialize(R &&range, SerializerFunction serializer) {

						if constexpr (SizeofSerializedObject == 0) {

							static_assert(SizeofSerializedObject > 0,
										  "List::SRI::serialize: SizeofSerializedObject "
										  "must be greater than zero.");

						} else if constexpr (!std::ranges::input_range<R>) {

							static_assert(std::ranges::input_range<R>,
										  "List::SRI::serialize: the first argument must be "
										  "a range that can be iterated.");

						} else if constexpr (!std::invocable<SerializerFunction &,
															 std::ranges::range_reference_t<R>>) {

							static_assert(std::invocable<SerializerFunction &,
														 std::ranges::range_reference_t<R>>,
										  "List::SRI::serialize: the serializer must be "
										  "callable with an element from the supplied range.");

						} else {

							using Serialized = std::remove_cvref_t<
								std::invoke_result_t<SerializerFunction &,
													 std::ranges::range_reference_t<R>>>;

							if constexpr (!Details::SerializedByteRange<Serialized>) {

								static_assert(Details::SerializedByteRange<Serialized>,
											  "List::SRI::serialize: the serializer must return "
											  "a sized range of std::byte, such as "
											  "std::array<std::byte, N> or "
											  "std::vector<std::byte>.");

							} else {

								if constexpr (std::ranges::sized_range<R>) {

									const auto size =
										Details::checked_list_size(std::ranges::size(range));

									return Details::serialize_known_size<SizeofSerializedObject>(
										std::forward<R>(range),
										serializer,
										size);

								} else if constexpr (std::ranges::forward_range<R>) {

									const auto size =
										Details::count_forward_range<SizeofSerializedObject>(range);

									return Details::serialize_known_size<SizeofSerializedObject>(
										std::forward<R>(range),
										serializer,
										size);

								} else {

									return Details::serialize_single_pass<SizeofSerializedObject>(
										std::forward<R>(range),
										serializer);
								}
							}
						}
					}

				} // namespace SRI

				namespace DSI {

					inline ListSize get_size(const std::vector<std::byte> &bytes,
											 std::size_t                   offset) {
						if (offset > bytes.size() || bytes.size() - offset < sizeof(ListSize)) {
							throw ByteError("The list offset is outside the serialized bytes.");
						}

						return Deserializer::read_uint16_t(bytes, offset);
					}

					namespace Details {

						template <std::size_t SizeofSerializedObject,
								  typename T,
								  typename DeserializerFunction>
						std::vector<T> deserialize_impl(const std::vector<std::byte> &bytes,
														DeserializerFunction         &deserializer,
														std::size_t                   offset) {

							if (offset > bytes.size() || bytes.size() - offset < sizeof(ListSize)) {

								throw ByteError("The list offset is outside the serialized bytes.");
							}

							const auto size = get_size(bytes, offset);

							constexpr auto max = std::numeric_limits<std::size_t>::max();

							if (size > (max - sizeof(ListSize)) / SizeofSerializedObject) {

								throw ByteError(
									"The serialized list size exceeds std::size_t limits.");
							}

							const auto elements_size =
								static_cast<std::size_t>(size) * SizeofSerializedObject;

							const auto serialized_size = sizeof(ListSize) + elements_size;

							if (bytes.size() - offset < serialized_size) {
								throw ByteError("The declared list size exceeds the number "
												"of available serialized bytes.");
							}

							std::vector<T> deserialized_values;
							deserialized_values.reserve(size);

							for (std::size_t index = 0; index < size; ++index) {

								deserialized_values.emplace_back(std::invoke(
									deserializer,
									bytes,
									get_offset_based_on_index<SizeofSerializedObject>(index,
																					  offset)));
							}

							return deserialized_values;
						}

					} // namespace Details

					template <std::size_t SizeofSerializedObject, typename DeserializerFunction>
					auto deserialize(const std::vector<std::byte> &bytes,
									 DeserializerFunction          deserializer,
									 std::size_t                   offset) {

						if constexpr (SizeofSerializedObject == 0) {

							static_assert(SizeofSerializedObject > 0,
										  "List::DSI::deserialize: SizeofSerializedObject "
										  "must be greater than zero.");

						} else if constexpr (!std::invocable<DeserializerFunction &,
															 const std::vector<std::byte> &,
															 std::size_t>) {

							static_assert(std::invocable<DeserializerFunction &,
														 const std::vector<std::byte> &,
														 std::size_t>,
										  "List::DSI::deserialize: the deserializer must "
										  "accept (const std::vector<std::byte>&, std::size_t).");

						} else if constexpr (std::is_void_v<std::invoke_result_t<
												 DeserializerFunction &,
												 const std::vector<std::byte> &,
												 std::size_t>>) {

							static_assert(
								!std::is_void_v<std::invoke_result_t<DeserializerFunction &,
																	 const std::vector<std::byte> &,
																	 std::size_t>>,
								"List::DSI::deserialize: the deserializer must "
								"return the object reconstructed from the bytes.");

						} else {

							using Result = std::invoke_result_t<DeserializerFunction &,
																const std::vector<std::byte> &,
																std::size_t>;

							using T = std::remove_cvref_t<Result>;

							if constexpr (!std::constructible_from<T, Result>) {

								static_assert(std::constructible_from<T, Result>,
											  "List::DSI::deserialize: the deserializer's "
											  "result must be usable to construct the "
											  "element type stored in the resulting vector.");

							} else {

								return Details::deserialize_impl<SizeofSerializedObject, T>(
									bytes,
									deserializer,
									offset);
							}
						}
					}

				} // namespace DSI

			} // namespace List
		} // namespace Types
	} // namespace Binary
} // namespace Karkinolution


namespace ListTDSI = Karkinolution::Binary::Types::List::DSI;
namespace ListTSRI = Karkinolution::Binary::Types::List::SRI;