#pragma once
#include <ctime>
#pragma pack (push, 1)

struct Receipt {
	int uid = 0;
	int user_UID = 0;
	int goods_index = 0;
	std::tm	purchase_date = {};
};

#pragma pack(pop) 
