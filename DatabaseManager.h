#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <iostream>

class DatabaseManager {
private:
    SQLHENV hEnv;
    SQLHDBC hDbc;
    SQLRETURN ret;

public:
    DatabaseManager();
    ~DatabaseManager();

    bool connect();                     
    void disconnect();                 
    bool isConnected();                
    void printError(const std::string& fn, SQLHANDLE handle, SQLSMALLINT type);
    SQLHDBC getDbc();            
};

#endif
