#include "app/synchronizer.hpp"

bool KarkinolutionView::Synchronizer::is_connected() const {
	return state.connected;
}

bool KarkinolutionView::Synchronizer::connect() {
	if (creature_creator.busy() || client.is_connected()) {
		return false;
	}
	if (!client.connect()) {
		state.connected = false;
		return false;
	}
	state.connected = false;
	creature_creator.acquire_world();
	return true;
}

bool KarkinolutionView::Synchronizer::disconnect() {
	if (creature_creator.busy()) {
		return false;
	}
	client.disconnect();
	state.connected = false;
	return true;
}

void KarkinolutionView::Synchronizer::create_creature() {
	if (!state.connected) {
		return;
	}
	creature_creator.create();
}

void KarkinolutionView::Synchronizer::poll() {
	if (state.connected && !client.is_connected()) {
		state.connected = false;
	}

	switch (creature_creator.poll()) {
		case CreatureCreator::PollResult::WORLD_READY:
			state.connected = true;
			break;
		case CreatureCreator::PollResult::FAILED:
			state.connected = false;
			client.disconnect();
			break;
		case CreatureCreator::PollResult::NONE:
		case CreatureCreator::PollResult::CREATURE_CREATED:
		case CreatureCreator::PollResult::CREATION_FAILED:
			break;
	}
}

void KarkinolutionView::Synchronizer::draw() {
	state_view.draw();
}
