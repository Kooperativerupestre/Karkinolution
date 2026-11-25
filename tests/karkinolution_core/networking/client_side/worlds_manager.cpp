#include "karkinolution/networking/client_side/world/worlds_manager.hpp"

#include "karkinolution/networking/frame/system/interpreter.hpp"
#include "karkinolution/networking/frame/system/motor.hpp"

#include <atomic>
#include <gtest/gtest.h>
#include <mutex>
#include <thread>

namespace {

	using Karkinolution::Networking::ClientSide::Client::SystemRequestData;
	using Karkinolution::Networking::ClientSide::Protocol::RequestFailure;
	using Karkinolution::Networking::ClientSide::Protocol::SystemSession;
	using Karkinolution::Networking::ClientSide::Protocol::WorldSession;
	using SystemResponse = Karkinolution::Networking::Frames::System::Models::SystemOutput::SystemOutput;
	using Karkinolution::Networking::ClientSide::World::SetWorldOutput;
	using Karkinolution::Networking::ClientSide::World::WorldId;
	using Karkinolution::Networking::ClientSide::World::WorldInexistenceReason;
	using Karkinolution::Networking::ClientSide::World::WorldsManager;
	using Karkinolution::Networking::ClientSide::World::WorldStorage;
	using Karkinolution::Networking::Frames::System::SystemOutputSubTypes;
	using Karkinolution::Networking::Frames::System::Interpreter::SRI::serialize;
	using Karkinolution::Networking::Frames::System::Interpreter::SystemOutput::SRI::make_config_output;
	using Karkinolution::Networking::Frames::System::Motor::make_system_output;

	class FakeClient {
		public:

			bool                 connected = true;
			std::vector<WorldId> world_ids;
			std::optional<bool>  config_result = true;

			bool connect() {
				return connected;
			}

			bool disconnect() {
				connected = false;
				return true;
			}

			bool is_connected() const {
				return connected;
			}

			SystemSession::ResponseResult request_system_scope(const SystemRequestData &request) {
				std::lock_guard lock{mutex};
				using namespace Karkinolution::Networking::Frames::System;

				if (request.sub_type == SystemCallSubTypes::GET_ALL_WORLD_IDS) {
					const auto payload = serialize(world_ids);
					return std::optional<SystemResponse>{
						make_system_output(SystemOutputSubTypes::AllWorldIds, 1, payload)};
				}

				if (request.sub_type == SystemCallSubTypes::CONFIG) {
					const auto payload = make_config_output({.could_connect = config_result.value_or(false)});
					return std::optional<SystemResponse>{
						make_system_output(SystemOutputSubTypes::ConfigOutput, 1, payload)};
				}

				return std::unexpected(RequestFailure::CANCEL);
			}

			WorldSession::ResponseResult
			request_world_scope(const Karkinolution::Networking::ClientSide::Client::WorldRequestData &) {
				return std::unexpected(RequestFailure::CANCEL);
			}

		private:

			std::mutex mutex;
	};

	TEST(WorldStorageTest, SyncProducesAnExactSetAndPreservesUnemplacedState) {
		WorldStorage               storage;
		const std::vector<WorldId> initial_ids{11, 23, 47};

		storage.sync(initial_ids);
		EXPECT_EQ(storage.see_all_world_ids().size(), initial_ids.size());
		for (const auto id : initial_ids) {
			EXPECT_TRUE(storage.has(id));
			EXPECT_EQ(storage.get_state(id), KShared::Functional::DoubleOptional::EXISTS_AND_HAS_NOT);
		}

		const auto world = Karkinolution::Networking::ClientSide::World::World{23};
		EXPECT_TRUE(storage.emplace(world));
		EXPECT_EQ(storage.get_state(23), KShared::Functional::DoubleOptional::EXISTS_AND_HAS);

		storage.sync({23, 59});
		EXPECT_FALSE(storage.has(11));
		EXPECT_FALSE(storage.has(47));
		EXPECT_TRUE(storage.has(23));
		EXPECT_TRUE(storage.has(59));
		EXPECT_EQ(storage.get_state(23), KShared::Functional::DoubleOptional::EXISTS_AND_HAS);
	}

	TEST(WorldStorageTest, GetWorldReportsTheTwoKindsOfNonexistence) {
		WorldStorage storage;
		storage.sync({31});

		const auto unknown = storage.get_world(30);
		EXPECT_FALSE(unknown.world);
		ASSERT_TRUE(unknown.inexistence_reason.has_value());
		EXPECT_EQ(unknown.inexistence_reason, WorldInexistenceReason::NO_EXISTS);

		const auto unemplaced = storage.get_world(31);
		EXPECT_FALSE(unemplaced.world);
		ASSERT_TRUE(unemplaced.inexistence_reason.has_value());
		EXPECT_EQ(unemplaced.inexistence_reason, WorldInexistenceReason::NO_EMPLACED);

		EXPECT_TRUE(storage.emplace(Karkinolution::Networking::ClientSide::World::World{31}));
		const auto placed = storage.get_world(31);
		EXPECT_TRUE(placed.world);
		EXPECT_FALSE(placed.inexistence_reason.has_value());
	}

	TEST(WorldStorageTest, ConcurrentSnapshotsAndUpdatesKeepStorageConsistent) {
		WorldStorage               storage;
		const std::vector<WorldId> ids{101, 103, 107, 109, 113, 127};
		storage.sync(ids);

		std::atomic<bool>        start = false;
		std::vector<std::thread> threads;
		for (int iteration = 0; iteration < 4; ++iteration) {
			threads.emplace_back([&] {
				while (!start.load(std::memory_order_acquire)) {
					std::this_thread::yield();
				}
				for (int pass = 0; pass < 100; ++pass) {
					storage.sync(ids);
					const auto snapshot = storage.see_all_world_ids();
					EXPECT_EQ(snapshot.size(), ids.size());
					for (const auto id : ids) {
						EXPECT_TRUE(storage.has(id));
					}
				}
			});
		}
		start.store(true, std::memory_order_release);
		for (auto &thread : threads) {
			thread.join();
		}

		EXPECT_EQ(storage.see_all_world_ids().size(), ids.size());
	}

	TEST(WorldsManagerTest, ConstructorSynchronizesIdsAndSetClearPreserveInvariants) {
		FakeClient client;
		client.world_ids = {41, 43};
		WorldsManager manager{client};

		EXPECT_EQ(manager.see_all_world_ids().size(), 2);
		EXPECT_FALSE(manager.has_current_world());
		EXPECT_FALSE(manager.current_world_exists());
		EXPECT_FALSE(manager.world_inexistence_is_broken());
		EXPECT_FALSE(manager.world_emplacement_is_broken());
		EXPECT_EQ(manager.set_current_world(999), SetWorldOutput::NO_EXISTS);
		EXPECT_EQ(manager.set_current_world(41), SetWorldOutput::EXISTS_AND_COULD);
		EXPECT_TRUE(manager.has_current_world());
		EXPECT_TRUE(manager.current_world_exists());
		EXPECT_FALSE(manager.world_emplacement_is_broken());
		EXPECT_EQ(manager.set_current_world(43), SetWorldOutput::ALREADY_IS_SET);
		EXPECT_TRUE(manager.see_current_world());

		EXPECT_EQ(manager.clear_current_world(),
				  Karkinolution::Networking::ClientSide::World::ClearCurrentWorldIdOutput::CLEANED);
		EXPECT_FALSE(manager.has_current_world());
		EXPECT_FALSE(manager.see_current_world());
		EXPECT_EQ(manager.clear_current_world(),
				  Karkinolution::Networking::ClientSide::World::ClearCurrentWorldIdOutput::IS_NOT_SET);
	}

	TEST(WorldsManagerTest, ConcurrentReadOperationsAreSerializedByTheManager) {
		FakeClient client;
		client.world_ids = {61, 67, 71};
		WorldsManager            manager{client};
		std::atomic<bool>        start = false;
		std::vector<std::thread> threads;

		for (int index = 0; index < 6; ++index) {
			threads.emplace_back([&] {
				while (!start.load(std::memory_order_acquire)) {
					std::this_thread::yield();
				}
				for (int pass = 0; pass < 100; ++pass) {
					EXPECT_EQ(manager.see_all_world_ids().size(), 3);
					EXPECT_TRUE(manager.has_world(61));
					EXPECT_FALSE(manager.current_world_exists());
					EXPECT_FALSE(manager.see_current_world());
				}
			});
		}
		start.store(true, std::memory_order_release);
		for (auto &thread : threads) {
			thread.join();
		}
	}

} // namespace
