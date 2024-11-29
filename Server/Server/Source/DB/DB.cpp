#pragma once
#include <Windows.h>
#include "DB.h"
#include "Query/Query.h"
#include "../TableManager/TableManager.h"
#include "../utility.h"
#include <sstream>
#include <iomanip>
#include <chrono>

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
	

	SQLPrepare(hStmt, (SQLWCHAR*)InsertAccount_Query, SQL_NTS);

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

int DB::InsertNewUser(const char* id, const char* nickname)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int uid = 0;

	int wNicknameLen = MultiByteToWideChar(CP_UTF8, 0, nickname, -1, NULL, 0);
	wchar_t* wNickname = new wchar_t[wNicknameLen];
	MultiByteToWideChar(CP_UTF8, 0, nickname, -1, wNickname, wNicknameLen);

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertNewUser) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return INVALIDKEY;
	}
	

	SQLPrepare(hStmt, (SQLWCHAR*)InsertNewUser_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, wcslen(wNickname), 0, (SQLPOINTER)wNickname, 0, NULL);

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

		delete[] wNickname;

		return uid;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (InsertNewUser)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	delete[] wNickname;
	return INVALIDKEY;
}

int DB::InsertNewUser(const char* id, const wchar_t* nickname)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int uid = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertNewUser) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return INVALIDKEY;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)InsertNewUser_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, wcslen(nickname), 0, (SQLWCHAR*)nickname, 0, NULL);

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

		return uid;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (InsertNewUser)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return INVALIDKEY;
}

bool DB::InsertUserGameRecords(const int uid)
{
	if (uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertUserGameRecords) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)InsertUserGameRecords_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (InsertUserGameRecords)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::InsertUserItem(const int owner_uid, const int itemCode, const int itemCount, const int itemType)
{
	if (owner_uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertUserItem) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)InsertUserItem_Query, SQL_NTS);

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

bool DB::InsertUserAttendance(const int uid, const int EventIndex)
{
	if (uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertUserAttendance) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)InsertUserAttendance_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&EventIndex), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (InsertUserAttendance)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

std::pair<bool, UserInfo> DB::SelectUserInfoForLogin(const char* id)
{
	UserInfo userInfo;

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (SelectUserInfoForLogin) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return { false,UserInfo() };
	}

	
	int state = true;

	SQLPrepare(hStmt, (SQLWCHAR*)SelectUserInfoForLogin_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (&state), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);

	retcode = SQLExecute(hStmt);

	bool result = false;

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
	{

		SQLLEN col1, col2, col3, col4, col5, col6, col7, col8, col9, col10, col11;
		TIMESTAMP_STRUCT date{};
		SQLINTEGER t = 0;
		SQLLEN bufLen = 0;
		userInfo.NickName = std::wstring(20, '\0');
		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			
			SQLGetData(hStmt, (int)UserInfo_Field::UID, SQL_C_LONG, &userInfo.UID, sizeof(userInfo.UID), &col1);
			SQLGetData(hStmt, (int)UserInfo_Field::AccountID, SQL_C_CHAR, &userInfo.AccountID[0], userInfo.AccountID.size(), &col2);
			SQLGetData(hStmt, (int)UserInfo_Field::NickName, SQL_C_WCHAR, &userInfo.NickName[0], userInfo.NickName.capacity(), &col3);
			SQLGetData(hStmt, (int)UserInfo_Field::UserLevel, SQL_C_LONG, &userInfo.UserLevel, sizeof(userInfo.UserLevel), &col4);
			SQLGetData(hStmt, (int)UserInfo_Field::PassLevel, SQL_C_LONG, &userInfo.PassLevel, sizeof(userInfo.PassLevel), &col5);
			SQLGetData(hStmt, (int)UserInfo_Field::UserTitle, SQL_C_LONG, &userInfo.UserTitle, sizeof(userInfo.UserTitle), &col6);
			SQLGetData(hStmt, (int)UserInfo_Field::ProfileSkin, SQL_C_LONG, &userInfo.ProfileSkin, sizeof(userInfo.ProfileSkin), &col7);
			SQLGetData(hStmt, (int)UserInfo_Field::Point, SQL_C_LONG, &userInfo.Point, sizeof(userInfo.Point), &col8);
			SQLGetData(hStmt, (int)UserInfo_Field::LastLoginTime, SQL_C_TYPE_TIMESTAMP, &date, sizeof(date), &col9);
			SQLGetData(hStmt, (int)UserInfo_Field::AttendanceDay, SQL_C_LONG, &userInfo.AttendanceDay, sizeof(userInfo.AttendanceDay), &col10);
			SQLGetData(hStmt, (int)UserInfo_Field::State, SQL_C_LONG, &t, sizeof(t), &col11);

			userInfo.date.tm_year = date.year;
			userInfo.date.tm_mon = date.month;
			userInfo.date.tm_mday = date.day;
			userInfo.characterCustomizing = SelectCharacterCustomizing(userInfo.UID);
			userInfo.State = t;
			userInfo.Gold = SelectUserItemCount(userInfo.UID, 100001);

			result = true;
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);

		return { result, userInfo };
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectUserInfoForLogin)\n");
	ErrorDisplay(hStmt, retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return { false,UserInfo() };
}

std::pair<bool, UserInfo> DB::SelectUserInfo(const int uid)
{
	if (uid == 0) {
		return { false, UserInfo()};
	}
	UserInfo userInfo;

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return { false,UserInfo() };
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)SelectUserInfo_Query, SQL_NTS);

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
		userInfo.characterCustomizing = SelectCharacterCustomizing(userInfo.UID);
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
	if (uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return 0;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)SelectUserAllCurrency_Query, SQL_NTS);

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

std::unordered_map<int, UserItem> DB::SelectUserAllItems(const int uid)
{
	if (uid == 0) {
		return std::unordered_map<int, UserItem>();;
	}
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;
	int zero = 0;
	std::unordered_map<int, UserItem> itemList;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (SelectUserAllItems) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return std::unordered_map<int, UserItem>();
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)SelectUserAllItems_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&zero), 0, NULL);

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
			itemList[item.item_UID] = item;
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return std::move(itemList);
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectUserAllItems)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return std::unordered_map<int, UserItem>();;
}

sCharacterCustomizing DB::SelectCharacterCustomizing(const int uid)
{
	if (uid == 0) {
		return sCharacterCustomizing();
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (SelectCharacterCustomizing) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return sCharacterCustomizing();
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)SelectCharacterCustomizing_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN bufLen = 0;
		sCharacterCustomizing sCustomizingData;
		int len = 0;
		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLLEN col1;
			SQLRETURN getLenRet = SQLGetData(hStmt, 1, SQL_C_LONG, &len, sizeof(len), &col1);
			if (getLenRet == SQL_NULL_DATA) {
				bufLen = 0;
			}
			std::vector<uint8_t> customizingData(len);
			bufLen = len;

			SQLGetData(hStmt, 2, SQL_C_BINARY, customizingData.data(), bufLen, &bufLen);

			sCustomizingData = DeserializationCharacterCustomizing(customizingData);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);

		return sCustomizingData;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectCharacterCustomizing)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);

	return sCharacterCustomizing();
}

int DB::SelectUserItemCount(const int uid, const int item_index)
{
	if (uid == 0) {
		return 0;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int count = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (SelectUserItemCount) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return 0;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)SelectUserItemCount_Query, SQL_NTS);

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

	DEBUGMSGNOPARAM("Execute Query Error : (SelectUserItemCount)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return 0;
}

std::vector<sDayAttendanceInfo> DB::SelectUserAttendanceEvent(const int uid, const int eventCode)
{
	if (uid == 0) {
		return std::vector<sDayAttendanceInfo>();
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	std::vector<sDayAttendanceInfo> dayAttendanceInfoList;

	int count = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (SelectUserAttendanceEvent) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return std::vector<sDayAttendanceInfo>();
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)SelectUserAttendanceEvent_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&eventCode), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLLEN col1, col2, col3;
			sDayAttendanceInfo dayAttendanceInfo{};
			char date[11] = { 0 };
			std::tm tDate = {};
			int day_number = 0;
			int is_rewarded = 0;

			SQLGetData(hStmt, 1, SQL_C_CHAR, date, sizeof(date), &col1);
			SQLGetData(hStmt, 2, SQL_C_LONG, &day_number, sizeof(day_number),&col2);
			SQLGetData(hStmt, 3, SQL_C_LONG, &is_rewarded, sizeof(is_rewarded), &col3);
			std::istringstream ssDate(date);
			ssDate >> std::get_time(&tDate, "%Y-%m-%d");
			dayAttendanceInfo.attendance_date = tDate;
			dayAttendanceInfo.day_number = day_number;
			dayAttendanceInfo.is_rewarded = is_rewarded;

			dayAttendanceInfoList.push_back(dayAttendanceInfo);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return dayAttendanceInfoList;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectUserAttendanceEvent)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return std::vector<sDayAttendanceInfo>();
}

sDayAttendanceInfo DB::SelectUserAttendanceEventLatest(const int uid, const int eventCode)
{
	if (uid == 0) {
		return sDayAttendanceInfo();
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int count = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (SelectUserAttendanceEventLatest) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return sDayAttendanceInfo();
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)SelectUserAttendanceEventLatest_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&eventCode), 0, NULL);

	retcode = SQLExecute(hStmt);

	sDayAttendanceInfo dayAttendanceInfo;

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLLEN col1, col2, col3;
			char date[11] = { 0 };
			std::tm tDate = {};
			int day_number = 0;
			int is_rewarded = 0;

			SQLGetData(hStmt, 1, SQL_C_CHAR, date, sizeof(date), &col1);
			SQLGetData(hStmt, 2, SQL_C_LONG, &day_number, sizeof(day_number), &col2);
			SQLGetData(hStmt, 3, SQL_C_LONG, &is_rewarded, sizeof(is_rewarded), &col3);

			std::istringstream ssDate(date);
			ssDate >> std::get_time(&tDate, "%Y-%m-%d");
			dayAttendanceInfo.attendance_date = tDate;
			dayAttendanceInfo.day_number = day_number;
			dayAttendanceInfo.is_rewarded = is_rewarded;
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return dayAttendanceInfo;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectUserAttendanceEventLatest)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return sDayAttendanceInfo();
}


int DB::SelectUserAttendanceToday(const int uid)
{
	if (uid == 0) {
		return 0;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int count = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (SelectUserAttendanceToday) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return 0;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)SelectUserAttendanceToday_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, 1, SQL_C_LONG, &count, sizeof(count), &col1);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return count;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectUserAttendanceToday)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return 0;
}

bool DB::UpdateUserConnectionState(const int uid, const int state)
{
	if (uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateUserGold) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)UpdateUserConnectionState_Query, SQL_NTS);

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
	if (uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateUserGold) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)UpsertUserItemCount_Query, SQL_NTS);

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
	if (uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateUserPoint) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)UpdateUserPoint_Query, SQL_NTS);

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

bool DB::UpdateBattleRecords(const int uid, const UserGameRecords& gameRecords)
{
	if (uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateBattleRecords) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)UpdateBattleRecords_Query, SQL_NTS);

	int c = gameRecords.KillCount.load();

	SQLBindAtomic_int(hStmt, 1, gameRecords.KillCount);
	SQLBindAtomic_int(hStmt, 2, gameRecords.DeathCount);
	SQLBindAtomic_int(hStmt, 3, gameRecords.Point);
	SQLBindAtomic_int(hStmt, 4, gameRecords.Weapon_Kill_Count);
	SQLBindAtomic_int(hStmt, 5, gameRecords.Punch_Kill_Count);
	SQLBindAtomic_int(hStmt, 6, gameRecords.Bomb_Count);
	SQLBindAtomic_int(hStmt, 7, gameRecords.Groggy_Count);
	SQLBindAtomic_int(hStmt, 8, gameRecords.Pick_Weapon_Count);
	SQLBindAtomic_int(hStmt, 9, gameRecords.Pick_Bomb_Count);
	SQLBindAtomic_int(hStmt, 10, gameRecords.Battle_Count);
	SQLBindAtomic_int(hStmt, 11, gameRecords.FITH_Team_Count);
	SQLBindAtomic_int(hStmt, 12, gameRecords.FITH_Indiv_Count);
	SQLBindAtomic_int(hStmt, 13, gameRecords.Victory_Count);
	SQLBindParameter(hStmt, 14, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;

	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateBattleRecords)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}


bool DB::UpdateUserItemCount(const int uid, const int item_index, const int valueOfChange)
{
	if (uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateUserGold) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)UpdateUserItemCount_Query, SQL_NTS);

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

bool DB::UpdateCharacterCustomizing(const int uid, const sCharacterCustomizing& characterCustomizing)
{
	if (uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	std::vector<uint8_t> serializationData = SerializationCharacterCustomizing(characterCustomizing);

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateCharacterCustomizing) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)UpdateCharacterCustomizing_Query, SQL_NTS);
	SQLLEN dataLen = serializationData.size();
	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, serializationData.size(), 0, (SQLPOINTER)serializationData.data(), serializationData.size(), &dataLen);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateCharacterCustomizing)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::UpdateCharacterCustomizing(const int uid, const std::vector<uint8_t> characterCustomizing)
{
	if (uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateCharacterCustomizing) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	

	const WCHAR* query = L"UPDATE UserInfo SET CharacterCustomizing = ? WHERE UID = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, characterCustomizing.size(), 0, (SQLPOINTER)characterCustomizing.data(), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateCharacterCustomizing)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::DeleteAcccount(const char* id)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (DeleteAcccount) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)DeleteAcccount_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (DeleteAcccount)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::DeleteUserInfo(const int uid)
{
	if (uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (DeleteUserInfo) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)DeleteUserInfo_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (DeleteUserInfo)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::DeleteUserItem(const int owner_uid, const int itemCode)
{
	if (owner_uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (DeleteUserInfo) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)DeleteUserItem_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&owner_uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCode), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (DeleteUserInfo)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::DeleteUserItemAll(const int owner_uid)
{
	if (owner_uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (DeleteUserItemAll) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)DeleteUserItemAll_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&owner_uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (DeleteUserItemAll()\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::DeleteUserAttendanceAll(const int user_uid)
{
	if (user_uid == 0) {
		return false;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (DeleteUserAttendanceAll) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)DeleteUserAttendanceAll_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&user_uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (DeleteUserAttendanceAll)\n");
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
	

	SQLPrepare(hStmt, (SQLWCHAR*)CheckValidateLogin_Query, SQL_NTS);

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