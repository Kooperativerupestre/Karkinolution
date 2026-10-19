#include "godot/extension/client/query/utils.hpp"

#include "godot/godot_string.hpp"

#include <string_view>

namespace KarkinolutionGodot {
	namespace World {
		namespace Utils {
			std::optional<ResponseFrame>
			execute_request(godot::Ref<GodotClient>                            client,
							const KarkinolutionSDK::Client::ClientRequestData &request_data) {
				if (client.is_null()) {
					return std::nullopt;
				}

				const auto response_result = client->request(request_data);
				if (!response_result) {
					const godot::Error err = response_result.error();
					client->emit_signal("error_occurred",
										static_cast<int64_t>(err),
										godot::String("Failed to execute request"));
					return std::nullopt;
				}

				const auto &frame = response_result.value();

				if (frame.type == Frames::Types::BinaryTypes::Error) {
					const std::string_view msg(reinterpret_cast<const char*>(frame.payload.data()),
											   frame.payload.size());
					client->emit_signal("error_occurred",
										static_cast<int64_t>(godot::FAILED),
										to_godot_string(msg));
					return std::nullopt;
				}

				return frame;
			}
		} // namespace Utils
	} // namespace World
} // namespace KarkinolutionGodot
