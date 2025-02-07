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
#include <unordered_set>
#include <set>
#include <queue>
#include <mutex>

#define SQLBindAtomic_int(stmt, index, value) int bindValue##index = value.load(); SQLBindParameter(hStmt, index, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (&bindValue##index), 0, NULL);

inline constexpr int AttendanceData_DaysToKeep = -60;
inline constexpr int ConnectionPoolSize = 5;

struct DB_Connection {
    SQLHDBC hDbc;
    bool isUse = false;
};

class DB {
public:

    DB(class TableManager* tableManager);
    ~DB();
	int Init();

    bool Connect(std::wstring odbc, std::wstring id, std::wstring password);
    void ErrorDisplay(SQLHSTMT& hStmt);
    Security* GetSecurity() { return mSecurity; }

    DB_Connection GetConnection();
    void ReturnConnection(DB_Connection connection);

    // INSERT
    ERROR_CODE InsertNewAcccount(const char* id, const char* password);
    int InsertNewUser(const char* id, const wchar_t* nickname);
    ERROR_CODE InsertUserGameRecords(const int uid);
    ERROR_CODE InsertUserItem(const int owner_uid, const int itemCode, const int itemCount, const int itemType);
    ERROR_CODE InsertUserAttendance(const int uid, const int EventIndex, const int day_count);
    ERROR_CODE InsertUserEventReward(const int uid, const int Eventcode);
    ERROR_CODE InsertUserPassReward(const int uid, const PassLevel& passLevelInfo);

    // SELECT
    int SelectAccountCount(const char* id);
    std::pair<ERROR_CODE, UserInfo> SelectUserInfoForLogin(const char* id);
    std::pair<ERROR_CODE, UserInfo> SelectUserInfo(const int uid);
    std::pair<ERROR_CODE, std::vector<UserItem>> SelectUserAllCurrency(const int uid);
    // unordered_map<item_UID, UserItem>
    std::pair<ERROR_CODE, std::unordered_map<int, UserItem>> SelectUserAllItems(const int uid);
    struct sCharacterCustomizing SelectCharacterCustomizing(const int uid);
    
    int SelectUserItemCount(const int uid, const int item_index);
    std::set<sDayAttendanceInfo> SelectUserAttendanceEvent(const int uid, const int eventCode);
    sDayAttendanceInfo SelectUserAttendanceEventLatest(const int uid, const int eventCode);
    int SelectUserAttendanceToday(const int uid);
    bool SelectUserAttendanceIsRewarded(const int uid, const int eventCode, const int dayCount);
    std::vector<struct UserMission> SelectUserMission(const int uid);

    UserPass SelectUserPass(const int uid, const int passCode);
    UserPassReward SelectUserPassReward(const int uid, const int passCode);

    // UPDATE
    ERROR_CODE UpdateUserConnectionState(const int uid, const int state);
    ERROR_CODE UpdateUserPoint(const int uid, const int valueOfChange);
    ERROR_CODE UpdateBattleRecords(const int uid, const UserGameRecords& gameRecords);
    ERROR_CODE UpdateUserItemCount(const int uid, const int item_index, const int valueOfChange);
    bool       UpdateUserAttendanceIsRewarded(const int uid, const int eventCode, const int dayCount, const int updateValue);
    ERROR_CODE UpdateUserEventReward_IsRewarded(const int uid, const int eventCode);
    ERROR_CODE UpdateUserPassReward_isRewarded(const int uid, const int passCode, const int level);

    // UPSERT
    ERROR_CODE UpsertUserItemCount(const int uid, const int item_index, const int valueOfChange);
    ERROR_CODE UpsertUserCurrency(const int uid, std::vector<UserItem> CurrencyList);
    ERROR_CODE UpsertCharacterCustomizing(const int uid, struct sCharacterCustomizing& characterCustomizing);
    ERROR_CODE UpsertUserMission(const int uid, std::vector<struct UserMission>& missionList);
    ERROR_CODE UpsertUserPass(const int uid, const int passCode);

    // DELETE
    ERROR_CODE DeleteAcccount(const char* id);
    ERROR_CODE DeleteUserInfo(const int uid);
    ERROR_CODE DeleteUserGameRecords(const int uid);
    ERROR_CODE DeleteUserItem(const int owner_uid, const int itemCode);
    ERROR_CODE DeleteUserItemAll(const int owner_uid);
    ERROR_CODE DeleteUserAttendanceAll(const int user_uid);
    ERROR_CODE DeleteUserAttendanceOutdated(const int day);

    ERROR_CODE CheckValidateLogin(const char* id, const char* password);

private:
    SQLHENV hEnv = NULL;
    std::queue<DB_Connection> mConnectionPool;
    std::mutex mConnectionPoolLock;
    std::condition_variable mConnectionPoolCondition;

    Security* mSecurity = nullptr;

    class TableManager* pTableManager = nullptr;
};