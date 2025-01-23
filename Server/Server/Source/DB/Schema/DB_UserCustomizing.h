#pragma once
#include "DB_UserItem.h"
#include "../../utility.h"
#include <string>
#include <unordered_map>

enum class UserCustomizing_Field {
	user_UID = 1,
	skin,
	head,
	face,
	back,
	emotion,
	userTitle,
	profileSkin
};

struct sCharacterCustomizing {
	std::unordered_map<CustomizingItemType, EquippedItem> customizingItems;

	void SetItem(CustomizingItemType type, EquippedItem item) {
		customizingItems[type] = item;
	}

	int GetItemCode(CustomizingItemType type) {
		return customizingItems[type].itemCode;
	}
};

