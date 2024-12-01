#pragma once
#pragma pack (push, 1)

enum class UserItem_Field {
	item_UID = 1,
	owner_UID,
	itemCode,
	count,
	itemType,
};


struct UserItem {
	int item_UID = 0;
	int	owner_UID = 0;
	int	itemCode = 0;
	int	count = 0;
	int	itemType = 0;
};

struct EquippedItem {
	int item_UID = 0;
	int itemCode = 0;
	int	itemType = 0;
};

#pragma pack(pop) 