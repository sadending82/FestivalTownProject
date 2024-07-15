#pragma once
#include <string>
// 테스트용 테이블

#pragma pack (push, 1)
struct ItemTable {
	int				UID;
	std::string		name;
	int				price;

	ItemTable() : UID(0), name(""), price(0) {}
	ItemTable(int u, const std::string& n, int p) : UID(u), name(n.c_str()), price(p) {}
};

#pragma pack(pop) 