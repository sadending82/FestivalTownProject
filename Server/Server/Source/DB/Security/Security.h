#pragma once
#include "../../utility.h"
#include "KISA_SHA256.h"

constexpr int SALT_LENGTH = 10;
constexpr int SHA256_DIGEST_LENGTH = 32; // = 256 byte

class Security {
public:
	std::string GenerateSalt();

	std::string HashingPassword(const std::string password, const std::string salt);

	bool VerifyPassword(std::string password, std::string hashedPassword, std::string salt);

private:
};