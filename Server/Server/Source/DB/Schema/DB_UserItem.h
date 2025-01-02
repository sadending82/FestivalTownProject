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

	UserItem() {};
	UserItem(int itemUID, int ownerUID, int itemcode, int itemCount, int itemtype)
		: item_UID(itemUID), owner_UID(ownerUID), itemCode(itemcode), count(itemCount), itemType(itemtype) {};

	// 재화 업데이트 용
	UserItem(int ownerUID, int itemcode, int itemCount)
		: owner_UID(ownerUID), itemCode(itemcode), count(itemCount) {};
};

struct EquippedItem {
	int item_UID = 0;
	int itemCode = 0;
	int	itemType = 0;
};

#pragma pack(pop) 