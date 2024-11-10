#pragma once
#include "Security.h"
#include <string>
#include <random>
#include <regex>

std::string Security::GenerateSalt()
{
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> distribution(0, 255);

	std::string hexSalt;

	for (int i = 0; i < SALT_LENGTH/2; ++i) {
		char hex[3];
		sprintf_s(hex, "%02x", distribution(generator));
		hexSalt += hex;
	}

	return hexSalt;
}

std::string Security::HashingPassword(const std::string password, const std::string salt)
{
	std::string str = password + salt;
	unsigned char hash[32];
	SHA256_Encrpyt(reinterpret_cast<const unsigned char*>(str.c_str()), str.size(), hash);

	std::string hashedStr;

	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		char hex[3];
		sprintf_s(hex, "%02x", hash[i]);
		hashedStr += hex;
	}

	return hashedStr;
}

bool Security::VerifyPassword(std::string password, std::string hashedPassword, std::string salt)
{
	if (hashedPassword != HashingPassword(password, salt)) {
		return false;
	}

	return true;
}

bool Security::VerifyString(const char* input)
{
	std::string str = input;

	if (str.length() == 0) return false;

	for (wchar_t c : str) {
		if (c == ' ') return false;
		if (iswpunct(c)) return false;
		if (0xAC00 <= c && c <= 0xD7A3) return false;
	}

	return true;
}

bool Security::VerifyEmail(const std::string email)
{

	std::regex email_pattern("[0-9a-zA-Z]([-_.]?[0-9a-zA-Z])*@[0-9a-zA-Z]([-_.]?[0-9a-zA-Z])*.[a-zA-Z]{2,3}");

	return std::regex_match(email.c_str(), email_pattern);
}
