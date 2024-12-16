#pragma once
#include "Security.h"
#include <string>
#include <random>
#include <regex>
#include <clocale>
#include <cwctype>

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

ERROR_CODE Security::CheckVerifyStringsForSignUp(const std::string ID, std::string password, const wchar_t* nickname)
{
	std::wstring wstNickName = nickname;
	std::transform(wstNickName.begin(), wstNickName.end(), wstNickName.begin(), std::towlower);

	if (FilteringID(ID) == false) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	if (FilteringPassword(password) == false) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	if (FilteringNickname(wstNickName.c_str()) == false) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	return ERROR_CODE::ER_NONE;
}

bool Security::FilteringEmail(const std::string email)
{

	std::regex email_pattern("[0-9a-zA-Z]([-_.]?[0-9a-zA-Z])*@[0-9a-zA-Z]([-_.]?[0-9a-zA-Z])*.[a-zA-Z]{2,3}");

	return std::regex_match(email.c_str(), email_pattern);
}

bool Security::FilteringID(const std::string ID)
{
	std::regex id_pattern("^(?=.*[a-zA-Z0-9])[a-zA-Z0-9\-_]{5,20}$");

	return std::regex_match(ID.c_str(), id_pattern);
}

bool Security::FilteringPassword(std::string password)
{
	std::regex password_pattern("^[^°¡-ÆR]{8,20}$");

	return std::regex_match(password.c_str(), password_pattern);
}

bool Security::FilteringNickname(const wchar_t* nickname)
{
	if (mSlangList.Search(nickname) == true) {
		return false;
	}

	std::wregex nickname_pattern(L"^(?=.*[a-zA-Z0-9°¡-ÆR])[a-zA-Z0-9°¡-ÆR\-_]{2,12}$");

	return std::regex_match(nickname, nickname_pattern);
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