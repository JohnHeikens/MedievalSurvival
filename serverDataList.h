#include "nbtSerializable.h"
#include "serverData.h"
#include "GlobalFunctions.h"
#pragma once
struct serverDataList: nbtSerializable {
	std::vector<serverData> servers = std::vector<serverData>();

	virtual void serializeValue(nbtSerializer& s) override;
	bool serialize(cbool& write);

};