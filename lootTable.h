#pragma once
#include "include/interface/idestructable.h"
#include <vector>
#include "constants.h"
#include <random>
#include "include/math/vectn.h"
#include "itemStack.h"
#include "include/filesystem/jsonReader.h"
#include <memory>
#include <map>
struct lootTable : IDestructable
{
	inline virtual std::vector<itemStack> roll(const dropData& data, std::mt19937& randomToUse) const
	{
		return std::vector<itemStack>();
	}
	static void dropLoot(const std::vector<itemStack>& rolledStacks, tickableBlockContainer* containerIn, cvec2& position, cvec2& speed = cvec2());
	static void dropLoot(const std::vector<itemStack>& rolledStacks, tickableBlockContainer* containerIn, cvec2& position, cfp& maxSpeed);
};

std::shared_ptr <lootTable> readLootTable(const std::wstring& path);
lootTable* readDrop(const jsonContainer& container);

extern std::map<std::wstring, std::shared_ptr <lootTable>> chestLootTables;