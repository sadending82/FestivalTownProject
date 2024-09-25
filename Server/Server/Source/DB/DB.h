#pragma once
#include "../DB/Security/Security.h"
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <fstream>

class DB {
public:

    DB();
    ~DB();
    void ShowError(SQLHANDLE handle, SQLSMALLINT handleType, RETCODE retcode);
	int Init();
    bool Connect(std::wstring odbc, std::wstring id, std::wstring password);

    bool UseAccountDB(SQLHSTMT& hStmt);
    bool UseGameDB(SQLHSTMT& hStmt);

    bool InsertNewAcccount(const char* id, const char* password);
    bool InsertNewUser(const char* id, const char* nickname);
    bool InsertRanking(const int uid);

    bool UpdateUserConnectionState(const int uid, const int state);
    bool UpdateUserGold(const int uid, const int valueOfChange);
    bool UpdateUserPoint(const int uid, const int valueOfChange);
    bool UpdateRanking(const int uid, const int killCount, const int deathCount, const int point);

    bool CheckValidateLogin(const char* id, const char* password);

private:
    SQLHENV hEnv = NULL;
    SQLHDBC hDbc = NULL;

    Security* mSecurity = nullptr;
};