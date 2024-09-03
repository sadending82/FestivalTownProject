//#pragma once
//#include "Security.h"
//
//std::string Security::GenerateSalt()
//{
//	unsigned char salt[SALT_LENGTH];
//	if (RAND_bytes(salt, SALT_LENGTH) == false) {
//		DEBUGMSGNOPARAM("Salt Generate Failed\n");
//	}
//
//	std::string hexSalt;
//
//	for (int i = 0; i < SALT_LENGTH; ++i) {
//		char hex[3];
//		sprintf_s(hex, "%02x", salt[i]);
//		hexSalt += hex;
//	}
//
//	return hexSalt;
//}
//
//std::string Security::HashingPassword(const std::string password, const std::string salt)
//{
//	std::string str = password + salt;
//	unsigned char hash[SHA256_DIGEST_LENGTH];
//
//	SHA256_CTX sha256;
//	SHA256_Init(&sha256);
//	SHA256_Update(&sha256, str.c_str(), str.size());
//	SHA256_Final(hash, &sha256);
//
//	std::string hashedStr;
//
//	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
//		char hex[3];
//		sprintf_s(hex, "%02x", hash[i]);
//		hashedStr += hex;
//	}
//
//	return hashedStr;
//}
//
//bool Security::VerifyPassword(std::string password, std::string hashedPassword, std::string salt)
//{
//	if (hashedPassword != HashingPassword(password, salt)) {
//		return false;
//	}
//
//	return true;
//}