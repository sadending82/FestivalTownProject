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
	int item_UID;
	int	owner_UID;
	int	itemCode;
	int	count;
	int	itemType;
};


#pragma pack(pop) 