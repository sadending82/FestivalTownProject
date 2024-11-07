#pragma once
#include <string>
#pragma pack (push, 1)

enum class Account_Field {
	ID,
	hashedPassword,
	salt
};


struct Account {
	std::string ID;
	std::string hashedPassword;
	std::string salt;
};


#pragma pack(pop) 