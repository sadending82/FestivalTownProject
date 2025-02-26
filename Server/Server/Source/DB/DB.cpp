#pragma once
#include <Windows.h>
#include <sstream>
#include <iomanip>
#include <chrono>
#include "DB.h"
#include "../TableManager/TableManager.h"
#include "../utility.h"

DB::DB(TableManager* tableManager)
{
	pTableManager = tableManager;
	mSecurity = new Security(tableManager->GetSlangList());
}

DB::~DB()
{
	while (!mConnectionPool.empty()) {
		SQLHDBC hDbc = mConnectionPool.front().hDbc;
		mConnectionPool.pop();
		SQLDisconnect(hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	}
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

	return 1;
}

bool DB::Connect(std::wstring odbc, std::wstring id, std::wstring password)
{
	Init();

	SQLRETURN retcode;

	for (int i = 0; i < ConnectionPoolSize; ++i) {

		DB_Connection connection;

		retcode = SQLSetEnvAttr(nullptr, SQL_ATTR_CONNECTION_POOLING, (SQLPOINTER)SQL_CP_ONE_PER_HENV, 0);
		if (retcode == SQL_ERROR) {
			DEBUGMSGNOPARAM("Enable ODBC connection pooling Fail\n", retcode);
			return false;
		}

		if ((retcode = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &connection.hDbc)) == SQL_ERROR) {
			return false;
		}

		if ((retcode = SQLSetConnectAttr(connection.hDbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0)) == SQL_ERROR) {

			return false;
		}

		retcode = SQLConnect(connection.hDbc, (wchar_t*)odbc.c_str(), SQL_NTS, (wchar_t*)id.c_str(), SQL_NTS, (wchar_t*)password.c_str(), SQL_NTS);

		if (retcode == SQL_ERROR) {
			DEBUGMSGNOPARAM("DB Connect Fail\n", retcode);
			return false;
		}

		mConnectionPool.push(connection);
	}

	DEBUGMSGNOPARAM("DB Connect Success\n", retcode);

	return true;
}

void DB::ErrorDisplay(SQLHSTMT& hStmt)
{
	SQLWCHAR sqlState[6];
	SQLINTEGER nativeError;
	SQLWCHAR messageText[256];
	SQLSMALLINT textLength;

	SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, messageText, sizeof(messageText), &textLength);

	std::wcout << L"SQLExecute Error State: " << sqlState << L", Native Error: " << nativeError
		<< L", Message: " << messageText << std::endl;
}


DB_Connection DB::GetConnection()
{
	std::unique_lock<std::mutex> lock(mConnectionPoolLock);
	mConnectionPoolCondition.wait(lock, [this] {return !mConnectionPool.empty(); });
	DB_Connection connection = mConnectionPool.front();
	//std::cout << mConnectionPool.size() << " GetConnection\n";
	mConnectionPool.pop();

	return connection;
}

void DB::ReturnConnection(DB_Connection connection)
{
	std::unique_lock<std::mutex> lock(mConnectionPoolLock);
	mConnectionPool.push(connection);
	mConnectionPoolCondition.notify_one();
	//std::cout << mConnectionPool.size() << " poool return\n";
}


ERROR_CODE DB::InsertNewAccount(const char* id, const char* password)
{
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	std::string salt = mSecurity->GenerateSalt();
	std::string hashedPassword = mSecurity->HashingPassword(password, salt);

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR){
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertNewAccount) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}
	

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL AccountDB.dbo.InsertAccount(?, ?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, hashedPassword.length(), 0, (SQLCHAR*)hashedPassword.c_str(), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, salt.length(), 0, (SQLCHAR*)salt.c_str(), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertNewAccount)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

int DB::InsertNewUser(const char* id, const wchar_t* nickname)
{
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int uid = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertNewUser) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return INVALIDKEY;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.InsertNewUser(?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, wcslen(nickname), 0, (SQLWCHAR*)nickname, 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, 1, SQL_C_LONG, &uid, sizeof(uid), &col1);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		//// Gold / Dia
		//InsertUserItem(uid, 100001, 0, 0);
		//InsertUserItem(uid, 100002, 0, 0);

		return uid;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertNewUser)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return INVALIDKEY;
}

ERROR_CODE DB::InsertUserGameRecords(const int uid)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertUserGameRecords) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.InsertUserGameRecords(?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertUserGameRecords)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::InsertUserItem(const int owner_uid, const int itemCode, const int itemCount, const int itemType)
{
	if (owner_uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;

	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertUserItem) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.InsertUserItem(?, ?, ?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&owner_uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCode), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCount), 0, NULL);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemType), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertUserItem)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::InsertUserAttendance(const int uid, const int EventIndex, const int day_count)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertUserAttendance) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.InsertUserAttendance(?, ?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&EventIndex), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&day_count), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertUserAttendance)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::InsertUserEventReward(const int uid, const int Eventcode)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertUserEventReward) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}



	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.InsertUserEventReward(?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&Eventcode), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertUserEventReward)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::InsertUserPassReward(const int uid, const PassLevel& passLevelInfo, const int level)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertUserPassReward) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}



	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.InsertUserPassReward(?, ?, ?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&passLevelInfo.pass), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&passLevelInfo.Pass_Type), 0, NULL);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&level), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertUserEventReward)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::InsertReceipt(const int uid, const int goods_index)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (InsertReceipt) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}



	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.InsertReceipt(?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&goods_index), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (InsertReceipt)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

int DB::SelectAccountCount(const char* id)
{
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int count = 999;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectAccountCount) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return 999;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL AccountDB.dbo.SelectAccountCount(?)}", SQL_NTS);

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
		ReturnConnection(connection);
		return count;
	}

	if (retcode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return 999;
	}
	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectAccountCount)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return 999;
}

std::pair<ERROR_CODE, UserInfo> DB::SelectUserInfoForLogin(const char* id)
{
	UserInfo userInfo;

	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserInfoForLogin) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return { ERROR_CODE::ER_DB_ERROR, UserInfo() };
	}



	retcode = SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectUserInfoForLogin(?)}", SQL_NTS);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
		std::cerr << "Error preparing the SQL statement." << std::endl;
		return { ERROR_CODE::ER_DB_ERROR ,UserInfo() };
	}

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);

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
			SQLGetData(hStmt, 1, SQL_C_LONG, &userInfo.UID, sizeof(userInfo.UID), &col1);
			SQLGetData(hStmt, 2, SQL_C_CHAR, &userInfo.AccountID[0], userInfo.AccountID.size(), &col2);
			SQLGetData(hStmt, 3, SQL_C_WCHAR, &userInfo.NickName[0], userInfo.NickName.capacity(), &col3);
			SQLGetData(hStmt, 4, SQL_C_LONG, &userInfo.UserLevel, sizeof(userInfo.UserLevel), &col4);
			SQLGetData(hStmt, 5, SQL_C_LONG, &userInfo.PassLevel, sizeof(userInfo.PassLevel), &col5);
			SQLGetData(hStmt, 6, SQL_C_LONG, &userInfo.Point, sizeof(userInfo.Point), &col6);
			SQLGetData(hStmt, 7, SQL_C_CHAR, date, sizeof(date), &col7);
			SQLGetData(hStmt, 8, SQL_C_LONG, &userInfo.AttendanceDay, sizeof(userInfo.AttendanceDay), &col8);
			SQLGetData(hStmt, 9, SQL_C_LONG, &t, sizeof(t), &col10);

			std::istringstream ssDate(date);
			ssDate >> std::get_time(&tDate, "%Y-%m-%d");

			userInfo.LastLogin = tDate;
			userInfo.State = t;

			result = true;
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return { ERROR_CODE::ER_NONE, userInfo };
	}

	if (retcode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return { ERROR_CODE::ER_NONE, UserInfo() };
	}
	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserInfoForLogin)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return { ERROR_CODE::ER_DB_ERROR ,UserInfo() };
}

std::pair<ERROR_CODE, UserInfo> DB::SelectUserInfo(const int uid)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return { ERROR_CODE::ER_DB_ERROR, UserInfo()};
	}
	UserInfo userInfo;
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserInfo) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return { ERROR_CODE::ER_DB_ERROR,UserInfo() };
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectUserInfo(?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1, col2, col3, col4, col5, col6, col7, col8, col9;
		TIMESTAMP_STRUCT date{};
		SQLINTEGER t = 0;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, (int)UserInfo_Field::UID, SQL_C_LONG, &userInfo.UID, sizeof(userInfo.UID), &col1);
			SQLGetData(hStmt, (int)UserInfo_Field::AccountID, SQL_C_CHAR, &userInfo.AccountID[0], userInfo.AccountID.size(), &col2);
			SQLGetData(hStmt, (int)UserInfo_Field::NickName, SQL_C_CHAR, &userInfo.NickName[0], userInfo.NickName.size(), &col3);
			SQLGetData(hStmt, (int)UserInfo_Field::UserLevel, SQL_C_LONG, &userInfo.UserLevel, sizeof(userInfo.UserLevel), &col4);
			SQLGetData(hStmt, (int)UserInfo_Field::PassLevel, SQL_C_LONG, &userInfo.PassLevel, sizeof(userInfo.PassLevel), &col5);
			SQLGetData(hStmt, (int)UserInfo_Field::Point, SQL_C_LONG, &userInfo.Point, sizeof(userInfo.Point), &col6);
			SQLGetData(hStmt, (int)UserInfo_Field::LastLoginTime, SQL_C_TYPE_TIMESTAMP, &date, sizeof(date), &col7);
			SQLGetData(hStmt, (int)UserInfo_Field::AttendanceDay, SQL_C_LONG, &userInfo.AttendanceDay, sizeof(userInfo.AttendanceDay), &col8);
			SQLGetData(hStmt, (int)UserInfo_Field::State, SQL_C_LONG, &t, sizeof(t), &col9);
		}

		userInfo.LastLogin.tm_year = date.year;
		userInfo.LastLogin.tm_mon = date.month;
		userInfo.LastLogin.tm_mday = date.day;
		userInfo.State = t;

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);

		return { ERROR_CODE::ER_NONE, userInfo };
	}

	if (retcode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return { ERROR_CODE::ER_NONE, UserInfo() };
	}
	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserInfo)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return { ERROR_CODE::ER_DB_ERROR,UserInfo() };
}

std::pair<ERROR_CODE, std::vector<UserItem>> DB::SelectUserAllCurrency(const int uid)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return { ERROR_CODE::ER_DB_ERROR, std::vector<UserItem>() };
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	std::vector<UserItem> itemList;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserAllCurrency) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return { ERROR_CODE::ER_DB_ERROR, std::vector<UserItem>() };
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectUserAllCurrency(?)}", SQL_NTS);

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
		ReturnConnection(connection);
		return { ERROR_CODE::ER_NONE, itemList };
	}

	if (retcode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return { ERROR_CODE::ER_DB_NO_DATA, std::vector<UserItem>() };
	}
	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserAllCurrency)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return { ERROR_CODE::ER_DB_ERROR, std::vector<UserItem>() };
}

std::pair<ERROR_CODE, std::unordered_map<int, UserItem>> DB::SelectUserAllItems(const int uid)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return { ERROR_CODE::ER_DB_ERROR, std::unordered_map<int, UserItem>() };
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;
	int zero = 0;
	std::unordered_map<int, UserItem> itemList;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserAllItems) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return{ ERROR_CODE::ER_DB_ERROR, std::unordered_map<int, UserItem>() };
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectUserAllItems(?)}", SQL_NTS);

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
			itemList[item.itemCode] = item;
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return{ ERROR_CODE::ER_NONE, itemList };
	}

	if (retcode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return { ERROR_CODE::ER_DB_NO_DATA, std::unordered_map<int, UserItem>() };
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserAllItems)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return { ERROR_CODE::ER_DB_ERROR, std::unordered_map<int, UserItem>() };
}

sCharacterCustomizing DB::SelectCharacterCustomizing(const int uid)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return sCharacterCustomizing();
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectCharacterCustomizing) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return sCharacterCustomizing();
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectCharacterCustomizing(?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN Len = 0;
		sCharacterCustomizing sCustomizingData;
		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			
			SQLGetData(hStmt, 1, SQL_C_LONG, &sCustomizingData.customizingItems[CustomizingItemType::CI_SKIN].itemCode, sizeof(int), NULL);
			sCustomizingData.customizingItems[CustomizingItemType::CI_SKIN].itemType = CustomizingItemType::CI_SKIN;

			SQLGetData(hStmt, 2, SQL_C_LONG, &sCustomizingData.customizingItems[CustomizingItemType::CI_HEAD].itemCode, sizeof(int), NULL);
			sCustomizingData.customizingItems[CustomizingItemType::CI_HEAD].itemType = CustomizingItemType::CI_HEAD;

			SQLGetData(hStmt, 3, SQL_C_LONG, &sCustomizingData.customizingItems[CustomizingItemType::CI_FACE].itemCode, sizeof(int), NULL);
			sCustomizingData.customizingItems[CustomizingItemType::CI_FACE].itemType = CustomizingItemType::CI_FACE;

			SQLGetData(hStmt, 4, SQL_C_LONG, &sCustomizingData.customizingItems[CustomizingItemType::CI_BACK].itemCode, sizeof(int), NULL);
			sCustomizingData.customizingItems[CustomizingItemType::CI_BACK].itemType = CustomizingItemType::CI_BACK;

			SQLGetData(hStmt, 5, SQL_C_LONG, &sCustomizingData.customizingItems[CustomizingItemType::CI_EMO].itemCode, sizeof(int), NULL);
			sCustomizingData.customizingItems[CustomizingItemType::CI_EMO].itemType = CustomizingItemType::CI_EMO;

		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);

		return sCustomizingData;
	}

	if (retcode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return sCharacterCustomizing();
	}
	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectCharacterCustomizing)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return sCharacterCustomizing();
}

int DB::SelectUserItemCount(const int uid, const int item_index)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return 0;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int count = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserItemCount) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return 0;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectUserItemCount(?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&item_index), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, 1, SQL_C_LONG, &count, sizeof(count), &col1);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return count;
	}

	std::cout << "Execute Query Error " << retcode  << ": (SelectUserItemCount) - " << uid  << " " << item_index << std::endl;;
	ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return 0;
}

std::set<sDayAttendanceInfo> DB::SelectUserAttendanceEvent(const int uid, const int eventCode)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return std::set<sDayAttendanceInfo>();
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	std::set<sDayAttendanceInfo> dayAttendanceInfoList;

	int count = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserAttendanceEvent) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return std::set<sDayAttendanceInfo>();
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectUserAttendanceEvent(?, ?)}", SQL_NTS);

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
		ReturnConnection(connection);
		return dayAttendanceInfoList;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserAttendanceEvent)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return std::set<sDayAttendanceInfo>();
}

sDayAttendanceInfo DB::SelectUserAttendanceEventLatest(const int uid, const int eventCode)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return sDayAttendanceInfo();
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int count = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserAttendanceEventLatest) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return sDayAttendanceInfo();
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectUserAttendanceEventLatest(?, ?)}", SQL_NTS);

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
		ReturnConnection(connection);
		return dayAttendanceInfo;
	}

	if (retcode != SQL_NULL_DATA) {
		DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserAttendanceEventLatest)\n", retcode); ErrorDisplay(hStmt);
	}
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return sDayAttendanceInfo();
}


int DB::SelectUserAttendanceToday(const int uid)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return 0;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int count = 0;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserAttendanceToday) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return 0;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectUserAttendanceToday(?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, 1, SQL_C_LONG, &count, sizeof(count), &col1);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return count;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserAttendanceToday)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return 0;
}

bool DB::SelectUserAttendanceIsRewarded(const int uid, const int eventCode, const int dayCount)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return true;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int isRewarded = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserAttendanceIsRewarded) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return true;
	}



	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectUserAttendanceIsRewarded(?, ?, ?)}", SQL_NTS);

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
		ReturnConnection(connection);
		return isRewarded;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserAttendanceIsRewarded)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return true;
}

std::vector<UserMission> DB::SelectUserMission(const int uid)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return std::vector<UserMission>();
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int isRewarded = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserMission) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return std::vector<UserMission>();
	}

	std::vector<UserMission> missionList;

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectUserMission(?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			UserMission missionInfo;
			SQL_DATE_STRUCT assigned_date;
			SQL_TIMESTAMP_STRUCT complete_time;

			SQLGetData(hStmt, 1, SQL_C_LONG, &missionInfo.user_UID, sizeof(int), NULL);
			SQLGetData(hStmt, 2, SQL_C_LONG, &missionInfo.mission_code, sizeof(int), NULL);
			SQLGetData(hStmt, 3, SQL_C_LONG, &missionInfo.mission_type, sizeof(int), NULL);
			SQLGetData(hStmt, 4, SQL_C_LONG, &missionInfo.mission_group, sizeof(int), NULL);
			SQLGetData(hStmt, 5, SQL_C_LONG, &missionInfo.mission_step, sizeof(int), NULL);
			SQLGetData(hStmt, 6, SQL_C_LONG, &missionInfo.progress, sizeof(int), NULL);
			SQLGetData(hStmt, 7, SQL_C_LONG, &missionInfo.required_count, sizeof(int), NULL);
			SQLGetData(hStmt, 8, SQL_C_TYPE_DATE, &assigned_date, sizeof(assigned_date), NULL);
			SQLGetData(hStmt, 9, SQL_C_LONG, &missionInfo.is_completed, sizeof(int), NULL);
			SQLGetData(hStmt, 10, SQL_C_TYPE_TIMESTAMP, &complete_time, sizeof(complete_time), NULL);

			SqlDateStruct_To_Tm(assigned_date, missionInfo.assigned_date);
			SqlTimestampStruct_To_Tm(complete_time, missionInfo.complete_time);

			missionList.push_back(missionInfo);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return missionList;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserMission)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return std::vector<UserMission>();
}

UserPass DB::SelectUserPass(const int uid, const int passIndex)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return UserPass();
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int isRewarded = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserMission) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return UserPass();
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectUserPass(?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&passIndex), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		UserPass passInfo;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {

			SQLGetData(hStmt, 1, SQL_C_LONG, &passInfo.user_UID, sizeof(int), NULL);
			SQLGetData(hStmt, 2, SQL_C_LONG, &passInfo.passIndex, sizeof(int), NULL);
			SQLGetData(hStmt, 3, SQL_C_LONG, &passInfo.passType, sizeof(int), NULL);
			SQLGetData(hStmt, 4, SQL_C_LONG, &passInfo.passLevel, sizeof(int), NULL);
			SQLGetData(hStmt, 5, SQL_C_LONG, &passInfo.passExp, sizeof(int), NULL);
			SQLGetData(hStmt, 6, SQL_C_LONG, &passInfo.daily_mission_exp, sizeof(int), NULL);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return passInfo;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserMission)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return UserPass();
}

std::unordered_map<int, UserPassReward> DB::SelectUserPassReward(const int uid, const int passIndex)
{

	if (uid == 0 || uid == INVALIDKEY) {
		return  std::unordered_map<int, UserPassReward>();
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int isRewarded = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (SelectUserPassReward) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return  std::unordered_map<int, UserPassReward>();
	}


	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.SelectUserPassReward(?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&passIndex), 0, NULL);

	retcode = SQLExecute(hStmt);

	std::unordered_map<int, UserPassReward> passRewardList;

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			UserPassReward passReward;
			char date[11] = { 0 };

			SQLGetData(hStmt, 1, SQL_C_LONG, &passReward.userUID, sizeof(int), NULL);
			SQLGetData(hStmt, 2, SQL_C_LONG, &passReward.passIndex, sizeof(int), NULL);
			SQLGetData(hStmt, 3, SQL_C_LONG, &passReward.passType, sizeof(int), NULL);
			SQLGetData(hStmt, 4, SQL_C_LONG, &passReward.level, sizeof(int), NULL);
			SQLGetData(hStmt, 5, SQL_C_LONG, &passReward.isRewarded, sizeof(int), NULL);


			passRewardList.insert({ passReward.level, passReward });
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return passRewardList;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (SelectUserPassReward)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return std::unordered_map<int, UserPassReward>();
}

ERROR_CODE DB::UpdateUserConnectionState(const int uid, const int state)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateUserConnectionState) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.UpdateUserConnectionState(?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&state), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateUserConnectionState)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::UpsertUserItemCount(const int uid, const int item_Code, const int valueOfChange)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpsertUserItemCount) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.UpsertUserItemCount(?, ?, ?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&item_Code), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&valueOfChange), 0, NULL);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemType), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpsertUserItemCount)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}


ERROR_CODE DB::UpdateUserPoint(const int uid, const int valueOfChange)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateUserPoint) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}


	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.UpdateUserPoint(?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&valueOfChange), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;

	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateUserPoint)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::UpdateBattleRecords(const int uid, const UserGameRecords& gameRecords)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateBattleRecords) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	
	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.UpdateBattleRecords(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)}", SQL_NTS);

	int c = gameRecords.KillCount.load();

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindAtomic_int(hStmt, 2, gameRecords.KillCount);
	SQLBindAtomic_int(hStmt, 3, gameRecords.DeathCount);
	SQLBindAtomic_int(hStmt, 4, gameRecords.Point);
	SQLBindAtomic_int(hStmt, 5, gameRecords.Weapon_Kill_Count);
	SQLBindAtomic_int(hStmt, 6, gameRecords.Punch_Kill_Count);
	SQLBindAtomic_int(hStmt, 7, gameRecords.Bomb_Count);
	SQLBindAtomic_int(hStmt, 8, gameRecords.Groggy_Count);
	SQLBindAtomic_int(hStmt, 9, gameRecords.Pick_Weapon_Count);
	SQLBindAtomic_int(hStmt, 10, gameRecords.Pick_Bomb_Count);
	SQLBindAtomic_int(hStmt, 11, gameRecords.Battle_Count);
	SQLBindAtomic_int(hStmt, 12, gameRecords.FITH_Team_Count);
	SQLBindAtomic_int(hStmt, 13, gameRecords.FITH_Indiv_Count);
	SQLBindAtomic_int(hStmt, 14, gameRecords.Victory_Count);
	SQLBindAtomic_int(hStmt, 15, gameRecords.Play_Time_Count);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;

	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateBattleRecords)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}


ERROR_CODE DB::UpdateUserItemCount(const int uid, const int item_index, const int valueOfChange)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int itemType = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateUserItemCount) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.UpdateUserItemCount (?, ?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&item_index), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&valueOfChange), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateUserItemCount)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::UpsertUserCurrency(const int uid, std::vector<UserItem> CurrencyList)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	if (CurrencyList.empty()) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpsertUserCurrency) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.UpsertUserCurrency (?, ?, ?)}", SQL_NTS);

	const int currencyListSize = CurrencyList.size();

	for (int i = 0; i < currencyListSize; ++i) {
		SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&CurrencyList[i].owner_UID), 0, NULL);
		SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&CurrencyList[i].itemCode), 0, NULL);
		SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&CurrencyList[i].count), 0, NULL);

		retcode = SQLExecute(hStmt);
	}


	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpsertUserCurrency)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::UpsertCharacterCustomizing(const int uid, sCharacterCustomizing& characterCustomizing)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;


	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpsertCharacterCustomizing) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	int skin_code = characterCustomizing.GetItemCode(CustomizingItemType::CI_SKIN);
	int head_code = characterCustomizing.GetItemCode(CustomizingItemType::CI_HEAD);
	int face_code = characterCustomizing.GetItemCode(CustomizingItemType::CI_FACE);
	int back_code = characterCustomizing.GetItemCode(CustomizingItemType::CI_BACK);
	int emotion_code = characterCustomizing.GetItemCode(CustomizingItemType::CI_EMO);

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.UpsertCharacterCustomizing(?, ?, ?, ?, ?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&skin_code), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&head_code), 0, NULL);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&face_code), 0, NULL);
	SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&back_code), 0, NULL);
	SQLBindParameter(hStmt, 6, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&emotion_code), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpsertCharacterCustomizing)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::UpsertUserMission(const int uid, std::vector<UserMission>& missionList)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}

	if (missionList.empty()) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpsertUserMission) \n", retcode);
		ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	const int missionListSize = missionList.size();

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.UpsertUserMission (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)}", SQL_NTS);
	for (int i = 0; i < missionListSize; ++i) {

		SQL_DATE_STRUCT assigned_date = {};
		SQL_TIMESTAMP_STRUCT complete_time = {};
		SQLLEN completeTimeNullFlag = SQL_NULL_DATA;

		Tm_To_SqlDateStruct(assigned_date, missionList[i].assigned_date);
		Tm_To_SqlTimestampStruct(complete_time, missionList[i].complete_time);

		retcode = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(SQL_INTEGER), 0, (SQLPOINTER)(&missionList[i].user_UID), 0, NULL);
		retcode = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(SQL_INTEGER), 0, (SQLPOINTER)(&missionList[i].mission_code), 0, NULL);
		retcode = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(SQL_INTEGER), 0, (SQLPOINTER)(&missionList[i].mission_type), 0, NULL);
		retcode = SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(SQL_INTEGER), 0, (SQLPOINTER)(&missionList[i].mission_group), 0, NULL);
		retcode = SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(SQL_INTEGER), 0, (SQLPOINTER)(&missionList[i].mission_step), 0, NULL);
		retcode = SQLBindParameter(hStmt, 6, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(SQL_INTEGER), 0, (SQLPOINTER)(&missionList[i].progress), 0, NULL);
		retcode = SQLBindParameter(hStmt, 7, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(SQL_INTEGER), 0, (SQLPOINTER)(&missionList[i].required_count), 0, NULL);
		retcode = SQLBindParameter(hStmt, 8, SQL_PARAM_INPUT, SQL_C_TYPE_DATE, SQL_TYPE_DATE, sizeof(SQL_DATE_STRUCT), 0, (SQLPOINTER)&assigned_date, 0, NULL);
		retcode = SQLBindParameter(hStmt, 9, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(SQL_INTEGER), 0, (SQLPOINTER)(&missionList[i].is_completed), 0, NULL);

		if (missionList[i].is_completed == true) {
			SQLBindParameter(hStmt, 10, SQL_PARAM_INPUT, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, sizeof(SQL_TIMESTAMP_STRUCT), 0, (SQLPOINTER)&complete_time, 0, &completeTimeNullFlag);
		}
		else {
			SQLBindParameter(hStmt, 10, SQL_PARAM_INPUT, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, sizeof(SQL_TIMESTAMP_STRUCT), 0, NULL, 0, &completeTimeNullFlag);
		}

		retcode = SQLExecute(hStmt);
	}

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpsertUserMission)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::UpsertUserPass(const int uid, UserPass& passInfo)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;


	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpsertUserPass) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}


	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.UpsertUserPass(?, ?, ?, ?, ?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&passInfo.passIndex), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&passInfo.passType), 0, NULL);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&passInfo.passLevel), 0, NULL);
	SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&passInfo.passExp), 0, NULL);
	SQLBindParameter(hStmt, 6, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&passInfo.daily_mission_exp), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpsertUserPass)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::UpsertUserCurrencyRecord(const int uid, const int item_index, const int produced_add_value, const int consumed_add_value)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;


	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpsertUserCurrencyRecord) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}


	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.UpsertUserCurrencyRecord(?, ?, ?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&item_index), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&produced_add_value), 0, NULL);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (SQLPOINTER)(&consumed_add_value), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpsertUserCurrencyRecord)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}


bool DB::UpdateUserAttendanceIsRewarded(const int uid, const int eventCode, const int dayCount, const int updateValue)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return true;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	unsigned char update_value = updateValue;
	unsigned char before_isRewarded = 1;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateUserAttendanceIsRewarded) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return true;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.UpdateUserAttendanceIsRewarded(?, ?, ?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&eventCode), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&dayCount), 0, NULL);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_UTINYINT, SQL_TINYINT, sizeof(unsigned char), 0, (void*)(&update_value), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLLEN col1;

			SQLGetData(hStmt, 1, SQL_C_UTINYINT, &before_isRewarded, sizeof(before_isRewarded), &col1);

			SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
			ReturnConnection(connection);

			return static_cast<bool>(before_isRewarded);
		}
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateUserAttendanceIsRewarded)\n", retcode); ErrorDisplay(hStmt);

	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return true;
}

ERROR_CODE DB::UpdateUserEventReward_IsRewarded(const int uid, const int eventCode)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_NO_DATA;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (UpdateUserEventReward_IsRewarded) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.UpdateUserEventReward_IsRewarded(?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&eventCode), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}
	else if (retcode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_NO_DATA;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (UpdateUserEventReward_IsRewarded)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::UpdateUserPassReward_isRewarded(const int uid, const int passIndex, const int level)
{
	return ERROR_CODE();
}


ERROR_CODE DB::DeleteAcccount(const char* id)
{
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteAcccount) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}
	
	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL AccountDB.dbo.DeleteAcccount(?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteAcccount)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::DeleteUserInfo(const int uid)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteUserInfo) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}


	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.DeleteUserInfo(?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteUserInfo)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::DeleteUserGameRecords(const int uid)
{
	if (uid == 0 || uid == INVALIDKEY) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteUserGameRecords) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}
	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.DeleteUserGameRecords(?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteUserGameRecords)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::DeleteUserItem(const int owner_uid, const int itemCode)
{
	if (owner_uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteUserInfo) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.DeleteUserItem(?, ?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&owner_uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCode), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteUserInfo)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::DeleteUserItemAll(const int owner_uid)
{
	if (owner_uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteUserItemAll) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.DeleteUserItemAll(?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&owner_uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteUserItemAll()\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::DeleteUserAttendanceAll(const int user_uid)
{
	if (user_uid == 0) {
		return ERROR_CODE::ER_DB_ERROR;
	}
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteUserAttendanceAll) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.DeleteUserAttendanceAll(?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&user_uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteUserAttendanceAll)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::DeleteUserAttendanceOutdated(const int day)
{
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (DeleteUserAttendanceOutdated) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL GameDB.dbo.DeleteUserAttendanceOutDated(?)}", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&day), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO || retcode == SQL_NO_DATA) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_NONE;
	}

	DEBUGMSGONEPARAM("Execute Query Error %d : (DeleteUserAttendanceOutdated)\n", retcode); ErrorDisplay(hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	ReturnConnection(connection);
	return ERROR_CODE::ER_DB_ERROR;
}

ERROR_CODE DB::CheckValidateLogin(const char* id, const char* password)
{
	DB_Connection connection = GetConnection();
	SQLHDBC hDbc = connection.hDbc;
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	std::string hashedPassword;
	std::string salt;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGONEPARAM("hStmt Error %d : (CheckValidateLogin) \n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	SQLPrepare(hStmt, (SQLWCHAR*)L"{CALL AccountDB.dbo.CheckValidateLogin(?)}", SQL_NTS);

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
		ReturnConnection(connection);
	}
	else {
		DEBUGMSGONEPARAM("Execute Query Error %d : (CheckValidateLogin)\n", retcode); ErrorDisplay(hStmt);
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		ReturnConnection(connection);
		return ERROR_CODE::ER_DB_ERROR;
	}

	if (mSecurity->VerifyPassword(password, hashedPassword, salt)) {
		return ERROR_CODE::ER_NONE;
	}

	return ERROR_CODE::ER_DB_ERROR;
}

void DB::SqlDateStruct_To_Tm(const SQL_DATE_STRUCT& date, std::tm& tm_date)
{
	tm_date.tm_year = date.year - 1900;
	tm_date.tm_mon = date.month - 1;
	tm_date.tm_mday = date.day;
}

void DB::SqlTimestampStruct_To_Tm(const SQL_TIMESTAMP_STRUCT& time, std::tm& tm_time)
{
	tm_time.tm_year = time.year - 1900;
	tm_time.tm_mon = time.month - 1;
	tm_time.tm_mday = time.day;

	tm_time.tm_hour = time.hour;
	tm_time.tm_min = time.minute;
	tm_time.tm_sec = time.second;
}

void DB::Tm_To_SqlDateStruct(SQL_DATE_STRUCT& date, const std::tm& tm_date)
{
	date.year = tm_date.tm_year + 1900;
	date.month = tm_date.tm_mon + 1;
	date.day = tm_date.tm_mday;
}

void DB::Tm_To_SqlTimestampStruct(SQL_TIMESTAMP_STRUCT& time, const std::tm& tm_time)
{
	time.year = tm_time.tm_year + 1900;
	time.month = tm_time.tm_mon + 1;
	time.day = tm_time.tm_mday;

	time.hour = tm_time.tm_hour;
	time.minute = tm_time.tm_min;
	time.second = tm_time.tm_sec;
	time.fraction = 0;
}