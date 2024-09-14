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

void DB::ShowError(SQLHANDLE handle, SQLSMALLINT handleType, RETCODE retcode) {
	SQLSMALLINT i = 0;
	SQLINTEGER native;
	SQLWCHAR state[10];
	SQLWCHAR text[256];
	SQLSMALLINT len;
	while (SQLGetDiagRec(handleType, handle, ++i, state, &native, text,
		sizeof(text), &len) == SQL_SUCCESS) {
		std::cout << "SQL Error: " << state << ": " << text << std::endl;
	}
}

bool DB::ReadConfig()
{
	std::string txt;

	std::ifstream file("Config/DBconfig.txt");
	if (!file) return false;
	int line = 0;

	while (line < 3 && std::getline(file, txt)) {
		if (line == 0) {
			std::string tmp = txt.substr(5);
			mOdbc.assign(tmp.begin(), tmp.end());
		}
		if (line == 1) {
			std::string tmp = txt.substr(3);
			mID.assign(tmp.begin(), tmp.end());
		}
		if (line == 2) {
			std::string tmp = txt.substr(3);
			mPassword.assign(tmp.begin(), tmp.end());
		}
		line++;
	}
	return true;
}

int DB::Init()
{
	mSecurity = new Security;

	SQLRETURN retcode;

	if (ReadConfig() == false) {
		DEBUGMSGNOPARAM("Read DB Config File Fail\n");
		return -1;
	}

	if (retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) == SQL_ERROR) {

		return -1;
	}

	if ((retcode = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0)) == SQL_ERROR) {
		
		return -1;
	}

	if ((retcode = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc)) == SQL_ERROR) {

		return -1;
	}

	if (Connect() == false) {
		return -1;
	}

	return 1;
}

bool DB::Connect()
{
	SQLRETURN retcode;

	if ((retcode = SQLSetConnectAttr(hDbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0)) == SQL_ERROR) {

		return false;
	}

	retcode = SQLConnect(hDbc, (wchar_t*)mOdbc.c_str(), SQL_NTS, (wchar_t*)mID.c_str(), SQL_NTS, (wchar_t*)mPassword.c_str(), SQL_NTS);

	if (retcode == SQL_ERROR) {
		DEBUGMSGNOPARAM("DB Connect Fail\n");
		return false;
	}

	DEBUGMSGNOPARAM("DB Connect Success\n");

	return true;
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

	if (mSecurity->VerifyString(id) == false || mSecurity->VerifyString(nickname) == false) {
		return false;
	}

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (InsertNewUser) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"INSERT INTO UserInfo (AccountID, NickName) VALUES (?, ?)";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, strlen(id), 0, (SQLCHAR*)id, 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_WCHAR, strlen(nickname), 0, (SQLCHAR*)nickname, 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
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

	const WCHAR* query = L"UPDATE UserInfo SET State = State + ? WHERE UID = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&state), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error : (UpdateUserGold)\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}

bool DB::UpdateUserGold(const int uid, const int valueOfChange)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR) {
		DEBUGMSGNOPARAM("hStmt Error : (UpdateUserGold) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseGameDB(hStmt);

	const WCHAR* query = L"UPDATE UserInfo SET Gold = Gold + ? WHERE UID = ?";

	SQLPrepare(hStmt, (SQLWCHAR*)query, SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&valueOfChange), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, sizeof(int), 0, (void*)(&uid), 0, NULL);

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
			SQLCHAR value1[HASHED_PASSWORD_LENGTH]; // hashedPassword
			SQLCHAR value2[SALT_LENGTH]; // salt
			SQLGetData(hStmt, 1, SQL_C_CHAR, value1, sizeof(value1), &len1);
			hashedPassword.assign(reinterpret_cast<char*>(value1), len1);

			SQLGetData(hStmt, 2, SQL_C_CHAR, value2, sizeof(value2), &len2);
			salt.assign(reinterpret_cast<char*>(value2), len2);
		}

		std::cout << hashedPassword << " - " << salt << std::endl;

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