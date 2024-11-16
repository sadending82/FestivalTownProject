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
	int item_UID = -1;
	int	owner_UID = -1;
	int	itemCode = -1;
	int	count = -1;
	int	itemType = -1;
};

struct EquippedItem {
	int item_UID = -1;
	int itemCode = -1;
};


#pragma pack(pop) 