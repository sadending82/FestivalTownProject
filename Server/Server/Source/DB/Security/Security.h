#pragma once
#include "../../utility.h"
#include "KISA_SHA256.h"
#include <string>

inline constexpr int SALT_LENGTH = 20;
inline constexpr int SHA256_DIGEST_LENGTH = 32; // = 256 byte
inline constexpr int HASHED_PASSWORD_LENGTH = SHA256_DIGEST_LENGTH * 2;

class Security {
public:
	std::string GenerateSalt();

	std::string HashingPassword(const std::string password, const std::string salt);

	bool VerifyPassword(std::string password, std::string hashedPassword, std::string salt);

	bool VerifyString(const char* input);

	bool VerifyEmail(const std::string email);

private:
};