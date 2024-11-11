#pragma once
#include <Windows.h>
#include "DB.h"
#include "../TableManager/TableManager.h"

DB::~DB()
{
	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	delete mSecurity;
}

int DB::Init()
{
	mSecurity = new Security;

	SQLRETURN retcode;

	if (retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_ERROR) {

		return -1;
	}

	if ((retcode = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0)) == SQL_ERROR) {
		
		return -1;
	}

	if ((retcode = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc)) == SQL_ERROR) {

		return -1;
	}

	return 1;
}

bool DB::Connect(std::wstring odbc, std::wstring id, std::wstring password)
{
	Init();

	SQLRETURN retcode;

	if ((retcode = SQLSetConnectAttr(hDbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0)) == SQL_ERROR) {

		return false;
	}

	retcode = SQLConnect(hDbc, (wchar_t*)odbc.c_str(), SQL_NTS, (wchar_t*)id.c_str(), SQL_NTS, (wchar_t*)password.c_str(), SQL_NTS);

	if (retcode == SQL_ERROR) {
		DEBUGMSGNOPARAM("DB Connect Fail\n");
		return false;
	}

	DEBUGMSGNOPARAM("DB Connect Success\n");

	return true;
}

void DB::ErrorDisplay(SQLHSTMT& hStmt, RETCODE retCode)
{
	SQLSMALLINT rec = 0;
	SQLINTEGER  error;

	wchar_t       wszMessage[1000] = { 0, };
	wchar_t       wszState[SQL_SQLSTATE_SIZE + 1];

	if (retCode == SQL_INVALID_HANDLE)
	{
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}

	while (SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, ++rec, wszState, &error, wszMessage, (short)(sizeof(wszMessage) / sizeof(wchar_t)), (short*)nullptr) == SQL_SUCCESS)
	{
		if (wcsncmp(wszState, L"01004", 5))
		{
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, error);
		}
	}
}

bool DB::UseAccountDB(SQLHSTMT& hStmt)
{
	SQLRETURN retcode;

	if ((retcode = SQLExecDirect(hStmt, (SQLWCHAR*)L"USE AccountDB", SQL_NTS)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : Can't Use AccountDB \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}
	return true;
}

bool DB::UseGameDB(SQLHSTMT& hStmt)
{
	SQLRETURN retcode;

	if ((retcode = SQLExecDirect(hStmt, (SQLWCHAR*)L"USE GameDB", SQL_NTS)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : Can't Use GameDB \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}
	return true;
}

bool DB::InsertNewAcccount(const char* id, const char* password)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	/*if (mSecurity->VerifyString(password) == false) {
		return false;
	}*/

	if (mSecurity->VerifyEmail(id) == false) {
		return false;
	}

	std::string salt = mSecurity->GenerateSalt();
	std::string hashedPassword = mSecurity->HashingPassword(password, salt);

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR){
		DEBUGMSGNOPARAM("hStmt Error : (InsertNewAcccount) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}
	UseAccountDB(hStmt);

	const WCHAR* query = L"INSERT INTO ACCOUNT VALUES (?, ?, ?)";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, hashedPassword.length(), 0, (SQLCHAR*)hashedPassword.c_str(), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, salt.length(), 0, (SQLCHAR*)salt.c_str(), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (InsertNewAcccount)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::InsertNewUser(const char* id, const char* nickname)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int uid;

	if (mSecurity->VerifyString(id) == false || mSecurity->VerifyString(nickname) == false) {
		return false;
	}

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertNewUser) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"INSERT INTO UserInfo (AccountID, NickName) OUTPUT INSERTED.uid as uid VALUES (?, ?)";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_WCHAR, strlen(nickname), 0, (SQLCHAR*)nickname, 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, 1, SQL_C_LONG, &uid, sizeof(uid), &col1);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);

		//// Gold / Dia
		//InsertUserItem(uid, 100001, 0, 0);
		//InsertUserItem(uid, 100002, 0, 0);

		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (InsertNewUser)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}



bool DB::InsertRanking(const int uid)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"INSERT INTO Ranking (UID) VALUES (?)";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (InsertRanking)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::InsertUserItem(const int owner_uid, const int itemCode, const int itemCount, const int itemType)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"INSERT INTO UserItem (owner_uid, itemCode, count, itemType) VALUES (?, ?, ?, ?)";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&owner_uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCode), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCount), 0, NULL);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemType), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (InsertUserItem)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

std::pair<bool, UserInfo> DB::SelectUserInfoForLogin(const char* id)
{
	UserInfo userInfo;

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return { false,UserInfo() };
	}

	UseGameDB(hStmt);
	int state = true;

	const WCHAR* query = L"UPDATE UserInfo\
							SET ConnectionState = ?\
							OUTPUT deleted.*\
							WHERE AccountID = ?;";


	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (&state), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1, col2, col3, col4, col5, col6, col7, col8, col9, col10, col11;
		TIMESTAMP_STRUCT date{};
		SQLINTEGER t = 0;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, (int)UserInfo_Field::UID, SQL_C_LONG, &userInfo.UID, sizeof(userInfo.UID), &col1);
			SQLGetData(hStmt, (int)UserInfo_Field::AccountID, SQL_C_CHAR, &userInfo.AccountID[0], userInfo.AccountID.size(), &col2);
			SQLGetData(hStmt, (int)UserInfo_Field::NickName, SQL_C_CHAR, &userInfo.NickName[0], userInfo.NickName.size(), &col3);
			SQLGetData(hStmt, (int)UserInfo_Field::UserLevel, SQL_C_LONG, &userInfo.UserLevel, sizeof(userInfo.UserLevel), &col4);
			SQLGetData(hStmt, (int)UserInfo_Field::PassLevel, SQL_C_LONG, &userInfo.PassLevel, sizeof(userInfo.PassLevel), &col5);
			SQLGetData(hStmt, (int)UserInfo_Field::UserTitle, SQL_C_LONG, &userInfo.UserTitle, sizeof(userInfo.UserTitle), &col6);
			SQLGetData(hStmt, (int)UserInfo_Field::ProfileSkin, SQL_C_LONG, &userInfo.ProfileSkin, sizeof(userInfo.ProfileSkin), &col7);
			SQLGetData(hStmt, (int)UserInfo_Field::Point, SQL_C_LONG, &userInfo.Point, sizeof(userInfo.Point), &col8);
			SQLGetData(hStmt, (int)UserInfo_Field::LastLoginTime, SQL_C_TYPE_TIMESTAMP, &date, sizeof(date), &col9);
			SQLGetData(hStmt, (int)UserInfo_Field::AttendanceDay, SQL_C_LONG, &userInfo.AttendanceDay, sizeof(userInfo.AttendanceDay), &col10);
			SQLGetData(hStmt, (int)UserInfo_Field::State, SQL_C_LONG, &t, sizeof(t), &col11);
		}

		userInfo.date.tm_year = date.year;
		userInfo.date.tm_mon = date.month;
		userInfo.date.tm_mday = date.day;

		userInfo.State = t;

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);

		userInfo.Gold = SelectUserItemCount(userInfo.UID, 100001);

		return { true, userInfo };
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectUserInfoForLogin)\n");
	ErrorDisplay(hStmt, retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return { false,UserInfo() };
}

std::pair<bool, UserInfo> DB::SelectUserInfo(const int uid)
{
	UserInfo userInfo;

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return { false,UserInfo() };
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"SELECT * FROM UserInfo WHERE UID = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1, col2, col3, col4, col5, col6, col7, col8, col9, col10, col11;
		TIMESTAMP_STRUCT date{};
		SQLINTEGER t = 0;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, (int)UserInfo_Field::UID, SQL_C_LONG, &userInfo.UID, sizeof(userInfo.UID), &col1);
			SQLGetData(hStmt, (int)UserInfo_Field::AccountID, SQL_C_CHAR, &userInfo.AccountID[0], userInfo.AccountID.size(), &col2);
			SQLGetData(hStmt, (int)UserInfo_Field::NickName, SQL_C_CHAR, &userInfo.NickName[0], userInfo.NickName.size(), &col3);
			SQLGetData(hStmt, (int)UserInfo_Field::UserLevel, SQL_C_LONG, &userInfo.UserLevel, sizeof(userInfo.UserLevel), &col4);
			SQLGetData(hStmt, (int)UserInfo_Field::PassLevel, SQL_C_LONG, &userInfo.PassLevel, sizeof(userInfo.PassLevel), &col5);
			SQLGetData(hStmt, (int)UserInfo_Field::UserTitle, SQL_C_LONG, &userInfo.UserTitle, sizeof(userInfo.UserTitle), &col6);
			SQLGetData(hStmt, (int)UserInfo_Field::ProfileSkin, SQL_C_LONG, &userInfo.ProfileSkin, sizeof(userInfo.ProfileSkin), &col7);
			SQLGetData(hStmt, (int)UserInfo_Field::Point, SQL_C_LONG, &userInfo.Point, sizeof(userInfo.Point), &col8);
			SQLGetData(hStmt, (int)UserInfo_Field::LastLoginTime, SQL_C_TYPE_TIMESTAMP, &date, sizeof(date), &col9);
			SQLGetData(hStmt, (int)UserInfo_Field::AttendanceDay, SQL_C_LONG, &userInfo.AttendanceDay, sizeof(userInfo.AttendanceDay), &col10);
			SQLGetData(hStmt, (int)UserInfo_Field::State, SQL_C_LONG, &t, sizeof(t), &col11);
		}

		userInfo.date.tm_year = date.year;
		userInfo.date.tm_mon = date.month;
		userInfo.date.tm_mday = date.day;

		userInfo.State = t;

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);

		userInfo.Gold = SelectUserItemCount(userInfo.UID, 100001);

		return { true, userInfo };
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectUserInfo)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return { false,UserInfo() };
}

bool DB::SelectUserAllCurrency(const int uid, std::vector<int>& currency_types_output, std::vector<int>& currency_amounts_output)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return 0;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"SELECT count, itemCode FROM UserItem WHERE owner_UID = ? AND itemType = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemType), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1, col2;
		int itemCode = 0;
		int amount = 0;
		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, 1, SQL_C_LONG, &amount, sizeof(amount), &col1);
			SQLGetData(hStmt, 2, SQL_C_LONG, &itemCode, sizeof(itemCode), &col2);
			currency_types_output.push_back(itemCode);
			currency_amounts_output.push_back(amount);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectItemCount)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::SelectUserAllItems(const int uid, std::vector<UserItem>& UserItems_output)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return 0;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"SELECT * FROM UserItem WHERE owner_UID = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1, col2, col3, col4, col5;
		UserItem item;
		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, (int)UserItem_Field::item_UID, SQL_C_LONG, &item.item_UID, sizeof(item.item_UID), &col1);
			SQLGetData(hStmt, (int)UserItem_Field::owner_UID, SQL_C_LONG, &item.owner_UID, sizeof(item.owner_UID), &col2);
			SQLGetData(hStmt, (int)UserItem_Field::itemCode, SQL_C_LONG, &item.itemCode, sizeof(item.itemCode), &col3);
			SQLGetData(hStmt, (int)UserItem_Field::count, SQL_C_LONG, &item.count, sizeof(item.count), &col4);
			SQLGetData(hStmt, (int)UserItem_Field::itemType, SQL_C_LONG, &item.itemType, sizeof(item.itemType), &col5);
			UserItems_output.push_back(item);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectItemCount)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

int DB::SelectUserItemCount(const int uid, const int item_index)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int count = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return 0;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"SELECT count FROM UserItem WHERE owner_UID = ? AND ItemCode = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&item_index), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, 1, SQL_C_LONG, &count, sizeof(count), &col1);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return count;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectItemCount)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return 0;
}

bool DB::UpdateUserConnectionState(const int uid, const int state)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateUserGold) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"UPDATE UserInfo SET ConnectionState = ? WHERE UID = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&state), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateUserConnectionState)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::UpsertUserItemCount(const int uid, const int item_Code, const int valueOfChange)
{
	// 아이템 테이블에서 중첩 불가능한지 필터링하는 코드 필요

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateUserGold) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"MERGE INTO UserItem AS a "
		L"USING (SELECT 1 AS match) AS b "
		L"ON a.itemCode = ? AND a.owner_UID = ? "
		L"WHEN MATCHED THEN "
		L"UPDATE SET count = count + ? "
		L"WHEN NOT MATCHED THEN "
		L"INSERT (owner_UID, itemCode, count, itemType) VALUES (?, ?, ?, ?);";
							

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&item_Code), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&valueOfChange), 0, NULL);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&item_Code), 0, NULL);
	SQLBindParameter(hStmt, 6, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&valueOfChange), 0, NULL);
	SQLBindParameter(hStmt, 7, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemType), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpsertUserItemCount)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}


bool DB::UpdateUserPoint(const int uid, const int valueOfChange)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateUserPoint) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"UPDATE UserInfo SET Point = Point + ? WHERE UID = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&valueOfChange), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;

	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateUserPoint)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::UpdateRanking(const int uid, const int killCount, const int deathCount, const int point)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"UPDATE Ranking SET KillCount = KillCount + ?, deathCount = deathCount + ?, Point = Point + ?  WHERE UID = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&killCount), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&deathCount), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&point), 0, NULL);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateRanking)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::UpdateUserItemCount(const int uid, const int item_index, const int valueOfChange)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateUserGold) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"UPDATE UserItem SET count = count + ? WHERE owner_UID = ? AND itemCode = ?";


	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&valueOfChange), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&item_index), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateUserItemCount)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::DeleteAcccount(const char* id)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseAccountDB(hStmt);

	const WCHAR* query = L"DELETE FROM Acount WHERE ID = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(id), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateRanking)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::DeleteUserInfo(const int uid)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"DELETE FROM UserInfo WHERE UID = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateRanking)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::DeleteUserItem(const int owner_uid, const int itemCode)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"DELETE FROM UserItem WHERE Item_UID = (SELECT TOP 1 Item_UID FROM UserItem WHERE owner_UID = ? and ItemCode = ? ORDER BY Item_UID ASC)";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(owner_uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(itemCode), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateRanking)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::CheckValidateLogin(const char* id, const char* password)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	std::string hashedPassword;
	std::string salt;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (CheckValidateLogin) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}
	UseAccountDB(hStmt);

	SQLPrepare(hStmt, (SQLWCHAR*)L"SELECT hashedPassword, salt FROM ACCOUNT WHERE ID = ?", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLLEN len1, len2;
			SQLCHAR value1[HASHED_PASSWORD_LENGTH+1]; // hashedPassword
			SQLCHAR value2[SALT_LENGTH+1]; // salt
			SQLGetData(hStmt, 1, SQL_C_CHAR, value1, sizeof(value1), &len1);
			hashedPassword.assign(reinterpret_cast<char*>(value1), len1);

			SQLGetData(hStmt, 2, SQL_C_CHAR, value2, sizeof(value2), &len2);
			salt.assign(reinterpret_cast<char*>(value2), len2);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	}
	else {
		DEBUGMSGNOPARAM("Execute Query Error : (CheckValidateLogin)\n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	if (mSecurity->VerifyPassword(password, hashedPassword, salt)) {
		return true;
	}

	return false;
}