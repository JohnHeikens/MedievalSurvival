#pragma once
#include "craftableSlotContainer.h"
struct smithingTableSlotContainer : craftableSlotContainer
{
	uiSlotContainer* smithingInputSlot = nullptr;
	uiSlotContainer* netheriteIngotSlot = nullptr;
	smithingTableSlotContainer();
	virtual bool addStack(itemStack& stack) override;
	virtual ~smithingTableSlotContainer() override;
	virtual itemStack calculateRecipeResult() override;
	virtual bool canAddStack(uiSlotContainer* containerToAddTo, itemStack& s);
	virtual void substractCraftingIngredients() override;
};