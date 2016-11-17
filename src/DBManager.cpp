#include "DBManager.h"
#include <string>
#include <iostream>

//using namespace std;

DBManager* DBManager::Pointer = NULL;

DBManager* DBManager::Instance()
{
	return DBManager::Pointer;
}

DBManager* DBManager::Create(const char* dbHost, unsigned int dbPort, const char* dbUser, const char* dbPass, const char* dbName)
{
	if (DBManager::Pointer == NULL)
	{
		try{
		DBManager::*Pointer = new DBManager(dbHost, dbPort, dbUser, dbPass, dbName);
		}catch(std::bad_alloc& ba){
                     std::cerr << "bad_alloc caught: ");
		}
		catch(...){
		
		}
	}
	return DBManager::*Pointer;
}

DBManager::DBManager(const char* dbHost, unsigned int dbPort, const char* dbUser, const char* dbPass, const char* dbName)
{
	try{
	this->dbHost = std::string(dbHost);
	}catch(...){
	std::string::~dbHost;
	}
	try{
	this->dbPort = dbPort;
	}catch(...){
	std::string::~dbPort;
	}
	try{
	this->dbUser = std::string(dbUser);
	}catch(...){
	std::string::~dbUser;
	}
	try{
	this->dbPass = std::string(dbPass);
	}catch(...){
	std::string::~dbPass;
	}
	try{
	this->dbName = std::string(dbName);
	}catch(...){
	std::string::~dbName;
	}

	this->dbHandle = mysql_init(0);
	}catch(...){
	
	
	
	
	
	}
	MYSQL *dbHandleErr = dbHandle;
	this->dbHandle = mysql_real_connect(dbHandle, this->dbHost.c_str(), this->dbUser.c_str(),
										this->dbPass.c_str(), this->dbName.c_str(), this->dbPort, NULL, 0);
	if (this->dbHandle == NULL)
	{
		printf("MySQL: %s", mysql_error(dbHandleErr));
		Thread::Wait(10*1000);
	}
}

DBManager::~DBManager()
{
	if (this->dbHandle != NULL)
	{
		mysql_close(this->dbHandle);
	}
}

bool DBManager::Query(const char* query)
{
	if (mysql_query(this->dbHandle, query))
	{
		printf("MySQL: Error with query\n");
		return false;
	}

	return true;
}

MYSQL_RES* DBManager::StoreResult()
{
	return mysql_store_result(this->dbHandle);
}

int DBManager::NumRows(MYSQL_RES* result)
{
	int rows = (int)mysql_num_rows(result);
	return rows;
}

MYSQL_ROW DBManager::FetchRow(MYSQL_RES* result)
{
	MYSQL_ROW row = mysql_fetch_row(result);
	return row;
}

void DBManager::FreeResult(MYSQL_RES* result)
{
	mysql_free_result(result);
}

int DBManager::ValidatePlayer(const char* Name, const char* Password, signed long long* UID)
{
	std::string Query;
	Query = "SELECT * FROM `users` WHERE username='";
	Query += Name;
	Query += "' AND password='";
	Query += Password;
	Query += "'";

	if (mysql_query(this->dbHandle, Query.c_str()))
	{
		printf("MySQL: Error with query\n");
		return -1;
	}

	MYSQL_RES* res = mysql_store_result(this->dbHandle);
	unsigned long long rows = mysql_num_rows(res);
	if (rows > 0)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		if (atoi(row[4]) == 1)
		{ mysql_free_result(res); return 1; } // Account Blocked
		if (atoi(row[5]) == 1)
		{ mysql_free_result(res); return 2; } // Already logged
		*UID = atoi(row[0]);
		mysql_free_result(res);
		return 0;
	}
	mysql_free_result(res); 
	return -1;
}
