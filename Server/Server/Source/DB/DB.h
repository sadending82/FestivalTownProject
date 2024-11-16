#pragma once
#include "../DB/Security/Security.h"
#include "Schema/Schemas.h"

#define _WINSOCKAPI_
#include <windows.h>  
// 이거 안쓰면 Flatbuffers 관련 헤더랑 충돌 일어남
#undef max
#undef min

#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <fstream>

#define SQLBindAtomic_int(stmt, index, value) int bindValue##index = value.load(); SQLBindParameter(hStmt, index, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (&bindValue##index), 0, NULL);


class DB {
public:

    DB(class TableManager* tableManager) :pTableManager(tableManager) {};
    ~DB();
	int Init();
    bool Connect(std::wstring odbc, std::wstring id, std::wstring password);
    void ErrorDisplay(SQLHSTMT& hStmt, RETCODE retCode);

    bool UseAccountDB(SQLHSTMT& hStmt);
    bool UseGameDB(SQLHSTMT& hStmt);


    // INSERT
    bool InsertNewAcccount(const char* id, const char* password);
    int InsertNewUser(const char* id, const char* nickname); // return UID
    bool InsertUserGameRecords(const int uid);
    bool InsertUserItem(const int owner_uid, const int itemCode, const int itemCount, const int itemType);

    // SELECT
    std::pair<bool, UserInfo> SelectUserInfoForLogin(const char* id);
    std::pair<bool, UserInfo> SelectUserInfo(const int uid);
    bool SelectUserAllCurrency(const int uid, std::vector<int>& currency_types_output, std::vector<int>& currency_amounts_output);
    // unordered_map<item_UID, UserItem>
    std::unordered_map<int, UserItem> SelectUserAllItems(const int uid);
    struct sCharacterCustomizing SelectCharacterCustomizing(const int uid);
    
    int SelectUserItemCount(const int uid, const int item_index);

    // UPDATE
    bool UpdateUserConnectionState(const int uid, const int state);
    bool UpdateUserPoint(const int uid, const int valueOfChange);
    bool UpdateBattleRecords(const int uid, const UserGameRecords& gameRecords);
    bool UpdateUserItemCount(const int uid, const int item_index, const int valueOfChange);
    bool UpdateCharacterCustomizing(const int uid, const struct sCharacterCustomizing& characterCustomizing);

    // UPSERT
    bool UpsertUserItemCount(const int uid, const int item_index, const int valueOfChange);

    // DELETE
    bool DeleteAcccount(const char* id);
    bool DeleteUserInfo(const int uid);
    bool DeleteUserItem(const int owner_uid, const int itemCode);

    bool CheckValidateLogin(const char* id, const char* password);

private:
    SQLHENV hEnv = NULL;
    SQLHDBC hDbc = NULL;

    Security* mSecurity = nullptr;

    class TableManager* pTableManager = nullptr;
};