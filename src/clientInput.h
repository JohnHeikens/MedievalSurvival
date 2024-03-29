#pragma once
#include <sfml/Window.hpp>
#include "keyID.h"
#include "keyHistoryEvent.h"
struct socketContainer;
struct clientInput {
	veci2 mousePositionPixels = veci2();

	int scrollDelta = 0;

	bool clicked[(byte)sf::Mouse::ButtonCount]{};
	bool clickReleased[(byte)sf::Mouse::ButtonCount]{};
	bool holding[(byte)sf::Mouse::ButtonCount]{};

	//the keys which were pressed (0 = start)
	std::vector<keyHistoryEvent> keyDownHistory = std::vector<keyHistoryEvent>();
	std::wstring textEntered = std::wstring();

	std::vector<vk> keysDown = std::vector<vk>();
	std::vector<vk> keysUp = std::vector<vk>();
	std::vector<vk> keysHolding = std::vector<vk>();

	bool serialize(const socketContainer& s);
	void addClientInput(const clientInput& newInput);
	bool holdingDownKey(cvk& key) const;
	bool pressedKey(cvk& key) const;
	//clear all data which should not be there if this input gets processed twice
	void clearTemporaryData();
};