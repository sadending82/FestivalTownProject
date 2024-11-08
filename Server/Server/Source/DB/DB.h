#pragma once
#include "../DB/Security/Security.h"
#include "Schema/Schemas.h"

#define _WINSOCKAPI_
#include <windows.h>  
// �̰� �Ⱦ��� Flatbuffers ���� ����� �浹 �Ͼ
#undef max
#undef min

#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <fstream>

class DB {
public:

    DB();
    ~DB();
	int Init();
    bool Connect(std::wstring odbc, std::wstring id, std::wstring password);
    void ErrorDisplay(SQLHSTMT& hStmt, RETCODE retCode);

    bool UseAccountDB(SQLHSTMT& hStmt);
    bool UseGameDB(SQLHSTMT& hStmt);

    bool InsertNewAcccount(const char* id, const char* password);
    bool InsertNewUser(const char* id, const char* nickname);
    bool InsertRanking(const int uid);
    bool InsertUserItem(const int owner_uid, const int itemCode, const int itemCount, const int itemType);

    std::pair<bool, UserInfo> SelectUserInfoForLogin(const char* id);
    std::pair<bool, UserInfo> SelectUserInfo(const int uid);
    
    int SelectUserItemCount(const int uid, const int item_index);

    bool UpdateUserConnectionState(const int uid, const int state);
    bool UpdateUserItemCount(const int uid, const int item_index, const int valueOfChange);
    bool UpdateUserPoint(const int uid, const int valueOfChange);
    bool UpdateRanking(const int uid, const int killCount, const int deathCount, const int point);

    bool CheckValidateLogin(const char* id, const char* password);

private:
    SQLHENV hEnv = NULL;
    SQLHDBC hDbc = NULL;

    Security* mSecurity = nullptr;
};