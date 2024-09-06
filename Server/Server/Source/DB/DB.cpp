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
			odbc.assign(tmp.begin(), tmp.end());
		}
		if (line == 1) {
			std::string tmp = txt.substr(3);
			id.assign(tmp.begin(), tmp.end());
		}
		if (line == 2) {
			std::string tmp = txt.substr(3);
			pw.assign(tmp.begin(), tmp.end());
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

	//InsertNewAcccount("newtest", "good");

	return 1;
}

bool DB::Connect()
{
	SQLRETURN retcode;

	if ((retcode = SQLSetConnectAttr(hDbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0)) == SQL_ERROR) {

		return false;
	}

	retcode = SQLConnect(hDbc, (wchar_t*)odbc.c_str(), SQL_NTS, (wchar_t*)id.c_str(), SQL_NTS, (wchar_t*)pw.c_str(), SQL_NTS);

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

bool DB::InsertNewAcccount(const char* id, const char* pw)
{
	SQLHSTMT hStmt = NULL;
	SQLRETURN retcode;

	std::string salt = mSecurity->GenerateSalt();
	std::string hashedPassword = mSecurity->HashingPassword(pw, salt);

	if (UseAccountDB(hStmt) == false) {
		return false;
	}

	if ((retcode = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt)) == SQL_ERROR){
		DEBUGMSGNOPARAM("hStmt Error : (InsertNewAcccount) \n");
		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return false;
	}

	UseAccountDB(hStmt);

	SQLPrepare(hStmt, (SQLWCHAR*)L"INSERT INTO ACCOUNT VALUES (?, ?, ?)", SQL_NTS);

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 50, 0, (SQLCHAR*)id, 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 256, 0, (SQLCHAR*)hashedPassword.c_str(), 0, NULL);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 50, 0, (SQLCHAR*)salt.c_str(), 0, NULL);

	retcode = SQLExecute(hStmt);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return true;
	}

	DEBUGMSGNOPARAM("Execute Query Error\n");
	SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
	return false;
}