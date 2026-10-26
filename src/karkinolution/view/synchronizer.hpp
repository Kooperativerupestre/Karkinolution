#pragma once

#include "karkinolution/networking/sdk/src/client/client.hpp"
#include "karkinolution/view/client/client.hpp"
#include "karkinolution/view/screen.hpp"
#include "karkinolution/view/world/world.hpp"

namespace KarkinolutionView {
	class Synchronizer {
		private:

			KarkinolutionView::Client::Client    client;
			KarkinolutionView::World::World      world;
			KarkinolutionView::Screen::State     state{false};
			KarkinolutionView::Screen::StateView state_view{state};

		public:

			Synchronizer() {}

			bool is_connected() const;
			bool connect();
			bool disconnect();


			void draw();


			KarkinolutionSDK::Client::ResponseResult
			request_and_sync(const KarkinolutionSDK::Client::RequestData &data);
	};
} // namespace KarkinolutionView