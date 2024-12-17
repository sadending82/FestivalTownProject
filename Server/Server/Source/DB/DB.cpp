#pragma once
#include <Windows.h>
#include "DB.h"
#include "Query/Query.h"
#include "../TableManager/TableManager.h"
#include "../utility.h"
#include <sstream>
#include <iomanip>
#include <chrono>

DB::DB(TableManager* tableManager)
{
	pTableManager = tableManager;
	mSecurity = new Security(tableManager->GetSlangList());
}

DB::~DB()
{
	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	delete mSecurity;
}

int DB::Init()
{

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
		DEBUGMSGNOPARAM("DB Connect Fail\n", retcode);
		return false;
	}

	DEBUGMSGNOPARAM("DB Connect Success\n", retcode);

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

ERROR_CODE DB::InsertNewAcccount(const char* id, const char* password)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	std::string salt = mSecurity->GenerateSalt();
	std::string hashedPassword = mSecurity->HashingPassword(password, salt);

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR){
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertNewAcccount) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}
	

	SQLPrepare(hStmt, (SQLWCHAR*)InsertAccount_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, hashedPassword.length(), 0, (SQLCHAR*)hashedPassword.c_str(), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, salt.length(), 0, (SQLCHAR*)salt.c_str(), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertNewAcccount)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

int DB::InsertNewUser(const char* id, const wchar_t* nickname)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int uid = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertNewUser) \n", retcode);
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

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertNewUser)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return INVALIDKEY;
}

ERROR_CODE DB::InsertUserGameRecords(const int uid)
{
	if (uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertUserGameRecords) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)InsertUserGameRecords_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertUserGameRecords)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::InsertUserItem(const int owner_uid, const int itemCode, const int itemCount, const int itemType)
{
	if (owner_uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertUserItem) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)InsertUserItem_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&owner_uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCode), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCount), 0, NULL);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemType), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertUserItem)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::InsertUserAttendance(const int uid, const int EventIndex, const int day_count)
{
	if (uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertUserAttendance) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)InsertUserAttendance_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&EventIndex), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&day_count), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertUserAttendance)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

int DB::SelectAccountCount(const char* id)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int count = 999;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectAccountCount) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return 999;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)SelectAccountCount_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);

	retcode = SQLExecute(hStmt);

	bool result = false;

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		SQLLEN col1;
		while (SQLFetch(hStmt) == SQL_SUCCESS) {

			SQLGetData(hStmt, 1, SQL_C_LONG, &count, sizeof(count), &col1);
			
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);

		return count;
	}

	ErrorDisplay(hStmt, retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	if (retcode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return 999;
	}
	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectAccountCount)\n", retcode);
	return 999;
}

std::pair<ERROR_CODE, UserInfo> DB::SelectUserInfoForLogin(const char* id)
{
	UserInfo userInfo;

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserInfoForLogin) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return { ERROR_CODE::ER_DB_ERROR, UserInfo() };
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
		char date[11] = { 0 };
		std::tm tDate = {};
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
			SQLGetData(hStmt, (int)UserInfo_Field::LastLoginTime, SQL_C_CHAR, date, sizeof(date), &col9);
			SQLGetData(hStmt, (int)UserInfo_Field::AttendanceDay, SQL_C_LONG, &userInfo.AttendanceDay, sizeof(userInfo.AttendanceDay), &col10);
			SQLGetData(hStmt, (int)UserInfo_Field::State, SQL_C_LONG, &t, sizeof(t), &col11);

			std::istringstream ssDate(date);
			ssDate >> std::get_time(&tDate, "%Y-%m-%d");

			userInfo.date = tDate;
			userInfo.characterCustomizing = SelectCharacterCustomizing(userInfo.UID);
			userInfo.State = t;
			userInfo.Gold = SelectUserItemCount(userInfo.UID, 100001);

			result = true;
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);

		return { ERROR_CODE::ER_NONE, userInfo };
	}

	ErrorDisplay(hStmt, retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	if (retcode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return { ERROR_CODE::ER_NONE, UserInfo() };
	}
	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserInfoForLogin)\n", retcode);
	return { ERROR_CODE::ER_DB_ERROR ,UserInfo() };
}

std::pair<ERROR_CODE, UserInfo> DB::SelectUserInfo(const int uid)
{
	if (uid == 0) {
		return { ERROR_CODE::ER_DB_ERROR, UserInfo()};
	}
	UserInfo userInfo;

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertRanking) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return { ERROR_CODE::ER_DB_ERROR,UserInfo() };
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

		return { ERROR_CODE::ER_NONE, userInfo };
	}

	ErrorDisplay(hStmt, retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	if (retcode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return { ERROR_CODE::ER_NONE, UserInfo() };
	}
	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserInfo)\n", retcode);
	return { ERROR_CODE::ER_DB_ERROR,UserInfo() };
}

std::pair<ERROR_CODE, std::vector<UserItem>> DB::SelectUserAllCurrency(const int uid)
{
	if (uid == 0) {
		return { ERROR_CODE::ER_DB_ERROR, std::vector<UserItem>() };
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	std::vector<UserItem> itemList;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserAllCurrency) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return { ERROR_CODE::ER_DB_ERROR, std::vector<UserItem>() };
	}



	SQLPrepare(hStmt, (SQLWCHAR*)SelectUserAllCurrency_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1, col2;
		int itemCode = 0;
		int amount = 0;
		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			UserItem item;

			SQLGetData(hStmt, 1, SQL_C_LONG, &amount, sizeof(amount), &col1);
			SQLGetData(hStmt, 2, SQL_C_LONG, &itemCode, sizeof(itemCode), &col2);
			item.itemCode = itemCode;
			item.count = amount;
			item.itemType = itemType;

			itemList.push_back(item);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return { ERROR_CODE::ER_NONE, itemList };
	}

	ErrorDisplay(hStmt, retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	if (retcode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return { ERROR_CODE::ER_DB_NO_DATA, std::vector<UserItem>() };
	}
	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserAllCurrency)\n", retcode);
	COUT << "UID : " << uid << ENDL;
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return { ERROR_CODE::ER_DB_ERROR, std::vector<UserItem>() };
}

std::pair<ERROR_CODE, std::unordered_map<int, UserItem>> DB::SelectUserAllItems(const int uid)
{
	if (uid == 0) {
		return { ERROR_CODE::ER_DB_ERROR, std::unordered_map<int, UserItem>() };
	}
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;
	int zero = 0;
	std::unordered_map<int, UserItem> itemList;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserAllItems) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return{ ERROR_CODE::ER_DB_ERROR, std::unordered_map<int, UserItem>() };
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
		return{ ERROR_CODE::ER_NONE, itemList };
	}

	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	if (retcode == SQL_NO_DATA) {
		return { ERROR_CODE::ER_DB_NO_DATA, std::unordered_map<int, UserItem>() };
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserAllItems)\n", retcode);
	return { ERROR_CODE::ER_DB_ERROR, std::unordered_map<int, UserItem>() };
}

sCharacterCustomizing DB::SelectCharacterCustomizing(const int uid)
{
	if (uid == 0) {
		return sCharacterCustomizing();
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectCharacterCustomizing) \n", retcode);
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

	ErrorDisplay(hStmt, retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	if (retcode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return sCharacterCustomizing();
	}
	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectCharacterCustomizing)\n", retcode);
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
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserItemCount) \n", retcode);
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

	COUT << "Execute Query Error " << retcode  << ": (SelectUserItemCount) - " << uid  << " " << item_index << ENDL;
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return 0;
}

std::set<sDayAttendanceInfo> DB::SelectUserAttendanceEvent(const int uid, const int eventCode)
{
	if (uid == 0) {
		return std::set<sDayAttendanceInfo>();
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	std::set<sDayAttendanceInfo> dayAttendanceInfoList;

	int count = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserAttendanceEvent) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return std::set<sDayAttendanceInfo>();
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

			dayAttendanceInfoList.insert(dayAttendanceInfo);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return dayAttendanceInfoList;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserAttendanceEvent)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return std::set<sDayAttendanceInfo>();
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
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserAttendanceEventLatest) \n", retcode);
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

			SQLGetData(hStmt, 1, SQL_C_CHAR, date, sizeof(date), &col1);
			SQLGetData(hStmt, 2, SQL_C_LONG, &day_number, sizeof(day_number), &col2);

			std::istringstream ssDate(date);
			ssDate >> std::get_time(&tDate, "%Y-%m-%d");
			dayAttendanceInfo.attendance_date = tDate;
			dayAttendanceInfo.day_number = day_number;
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return dayAttendanceInfo;
	}

	if (retcode != SQL_NULL_DATA)
		DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserAttendanceEventLatest)\n", retcode);
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
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserAttendanceToday) \n", retcode);
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

	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserAttendanceToday)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return 0;
}

bool DB::SelectUserAttendanceIsRewarded(const int uid, const int eventCode, const int dayCount)
{
	if (uid == 0) {
		return true;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int isRewarded = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserAttendanceIsRewarded) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}



	SQLPrepare(hStmt, (SQLWCHAR*)SelectUserAttendanceIsRewarded_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&eventCode), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&dayCount), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, 1, SQL_C_LONG, &isRewarded, sizeof(isRewarded), &col1);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return isRewarded;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserAttendanceIsRewarded)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return true;
}

ERROR_CODE DB::UpdateUserConnectionState(const int uid, const int state)
{
	if (uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateUserGold) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)UpdateUserConnectionState_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&state), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateUserConnectionState)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::UpsertUserItemCount(const int uid, const int item_Code, const int valueOfChange)
{
	if (uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateUserGold) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
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
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpsertUserItemCount)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}


ERROR_CODE DB::UpdateUserPoint(const int uid, const int valueOfChange)
{
	if (uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateUserPoint) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)UpdateUserPoint_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&valueOfChange), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;

	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateUserPoint)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::UpdateBattleRecords(const int uid, const UserGameRecords& gameRecords)
{
	if (uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateBattleRecords) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
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
		return ERROR_CODE::ER_NONE;

	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateBattleRecords)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}


ERROR_CODE DB::UpdateUserItemCount(const int uid, const int item_index, const int valueOfChange)
{
	if (uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateUserGold) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)UpdateUserItemCount_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&valueOfChange), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&item_index), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateUserItemCount)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::UpdateCharacterCustomizing(const int uid, const sCharacterCustomizing& characterCustomizing)
{
	if (uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	std::vector<uint8_t> serializationData = SerializationCharacterCustomizing(characterCustomizing);

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateCharacterCustomizing) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)UpdateCharacterCustomizing_Query, SQL_NTS);
	SQLLEN dataLen = serializationData.size();
	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, serializationData.size(), 0, (SQLPOINTER)serializationData.data(), serializationData.size(), &dataLen);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateCharacterCustomizing)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::UpdateCharacterCustomizing(const int uid, const std::vector<uint8_t> characterCustomizing)
{
	if (uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateCharacterCustomizing) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	

	const WCHAR* query = L"UPDATE UserInfo SET CharacterCustomizing = ? WHERE UID = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, characterCustomizing.size(), 0, (SQLPOINTER)characterCustomizing.data(), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateCharacterCustomizing)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

bool DB::UpdateUserAttendanceIsRewarded(const int uid, const int eventCode, const int dayCount, const int updateValue)
{
	if (uid == 0) {
		return true;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	unsigned char update_value = updateValue;
	unsigned char before_isRewarded = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateUserAttendanceIsRewarded) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)UpdateUserAttendanceIsRewarded_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_UTINYINT, SQL_TINYINT, sizeof(unsigned char), 0, (void*)(&update_value), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&eventCode), 0, NULL);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&dayCount), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLLEN col1;

			SQLGetData(hStmt, 1, SQL_C_UTINYINT, &before_isRewarded, sizeof(before_isRewarded), &col1);

			SQLFreeHandle(SQL_HANDLE_DBC, hStmt);

			return static_cast<bool>(before_isRewarded);
		}
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateUserAttendanceIsRewarded)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return true;
}

ERROR_CODE DB::DeleteAcccount(const char* id)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteAcccount) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}
	

	SQLPrepare(hStmt, (SQLWCHAR*)DeleteAcccount_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteAcccount)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::DeleteUserInfo(const int uid)
{
	if (uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteUserInfo) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)DeleteUserInfo_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteUserInfo)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::DeleteUserGameRecords(const int uid)
{
	if (uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteUserGameRecords) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)DeleteUserGameRecords_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteUserGameRecords)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::DeleteUserItem(const int owner_uid, const int itemCode)
{
	if (owner_uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteUserInfo) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)DeleteUserItem_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&owner_uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCode), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteUserInfo)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::DeleteUserItemAll(const int owner_uid)
{
	if (owner_uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteUserItemAll) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)DeleteUserItemAll_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&owner_uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteUserItemAll()\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::DeleteUserAttendanceAll(const int user_uid)
{
	if (user_uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteUserAttendanceAll) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)DeleteUserAttendanceAll_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&user_uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteUserAttendanceAll)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::DeleteUserAttendanceOutdated(const int day)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteUserAttendanceOutdated) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)DeleteUserAttendanceOutDated_Query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&day), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO || retcode == SQL_NO_DATA) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteUserAttendanceOutdated)\n", retcode);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::CheckValidateLogin(const char* id, const char* password)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	std::string hashedPassword;
	std::string salt;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (CheckValidateLogin) \n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
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
		DEBUGMSGONEPARAM("Execute Query Error %d : (CheckValidateLogin)\n", retcode);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return ERROR_CODE::ER_DB_ERROR;
	}

	if (mSecurity->VerifyPassword(password, hashedPassword, salt)) {
		return ERROR_CODE::ER_NONE;
	}

	return ERROR_CODE::ER_DB_ERROR;
}