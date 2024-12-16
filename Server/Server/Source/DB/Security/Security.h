#pragma once
#include "../../utility.h"
#include "KISA_SHA256.h"
#include <string>

inline constexpr int SALT_LENGTH = 20;
inline constexpr int SHA256_DIGEST_LENGTH = 32; // = 256 byte
inline constexpr int HASHED_PASSWORD_LENGTH = SHA256_DIGEST_LENGTH * 2;

class Security {
public:
	Security(Trie& SlangList) : mSlangList(SlangList){}

	std::string GenerateSalt();

	std::string HashingPassword(const std::string password, const std::string salt);

	bool VerifyPassword(std::string password, std::string hashedPassword, std::string salt);

	ERROR_CODE CheckVerifyStringsForSignUp(const std::string ID, std::string password, const wchar_t* nickname);

	bool FilteringEmail(const std::string email);

	bool FilteringID(const std::string ID);

	bool FilteringPassword(std::string password);

	bool FilteringNickname(const wchar_t* nickname);

	bool VerifyString(const char* input);

	void SetSlangList(Trie& SlangList) { mSlangList = SlangList; }
private:

	Trie& mSlangList;
};