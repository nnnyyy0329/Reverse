#pragma once
#include "appframe.h"
#include "Item.h"

class ItemManager
{
public:
	ItemManager();
	virtual ~ItemManager();

private:
	std::vector<std::shared_ptr<Item>> _itemList;
};

