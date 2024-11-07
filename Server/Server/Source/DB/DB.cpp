#pragma once
#include <Windows.h>
#include "DB.h"


DB::DB()
{
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

	if (mSecurity->VerifyString(id) == false) {
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

		// Gold / Dia
		InsertUserItem(uid, 100001, 0, 0);
		InsertUserItem(uid, 100002, 0, 0);

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

		userInfo.Gold = SelectGold(userInfo.UID);

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

		userInfo.Gold = SelectGold(userInfo.UID);

		return { true, userInfo };
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectUserInfo)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return { false,UserInfo() };
}

int DB::SelectGold(const int uid)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int gold = 0;
	int itemCode = 100001;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return 0;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"SELECT count FROM UserItem WHERE owner_UID = ? AND ItemCode = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCode), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, 1, SQL_C_LONG, &gold, sizeof(gold), &col1);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return gold;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectGold)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return 0;
}

int DB::SelectDia(const int uid)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	int dia = 0;
	int itemCode = 100002;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertRanking) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return 0;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"SELECT count FROM UserItem WHERE UID = ? AND ItemCode = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCode), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLLEN col1;

		while (SQLFetch(hStmt) == SQL_SUCCESS) {
			SQLGetData(hStmt, 1, SQL_C_LONG, &dia, sizeof(dia), &col1);
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return dia;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (SelectDia)\n");
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

bool DB::UpdateUserGold(const int uid, const int valueOfChange)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	const int itemCode = 100001;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateUserGold) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"UPDATE UserItem SET count = count + ? WHERE owner_UID = ? AND ItemCode = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&valueOfChange), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCode), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateUserGold)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::UpdateUserDia(const int uid, const int valueOfChange)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	const int itemCode = 100002;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateUserGold) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"UPDATE UserItem SET ItemCount = ItemCount + ? WHERE UID = ? AND ItemCode = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&valueOfChange), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&itemCode), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateUserGold)\n");
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