#pragma once
#include "Security.h"

std::string Security::GenerateSalt()
{
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> distribution(0, 255);

	std::string hexSalt;

	for (int i = 0; i < SALT_LENGTH; ++i) {
		char hex[3];
		sprintf_s(hex, "%02x", distribution(generator));
		hexSalt += hex;
	}

	return hexSalt;
}

std::string Security::HashingPassword(const std::string password, const std::string salt)
{
	std::string str = password + salt;
	/*unsigned char hash[32];

	SHA256_Encrpyt(reinterpret_cast<const unsigned char*>(str.c_str()), str.size(), hash);

	std::string hashedStr;

	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		char hex[3];
		sprintf_s(hex, "%02x", hash[i]);
		hashedStr += hex;
	}

	return hashedStr;*/
	return str;
}

bool Security::VerifyPassword(std::string password, std::string hashedPassword, std::string salt)
{
	if (hashedPassword != HashingPassword(password, salt)) {
		return false;
	}

	return true;
}