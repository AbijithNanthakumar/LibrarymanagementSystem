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

    bool connect();                     // Connect to SQL Server
    void disconnect();                 // Disconnect
    bool isConnected();                // Check connection status
    void printError(const std::string& fn, SQLHANDLE handle, SQLSMALLINT type);
    SQLHDBC getDbc();                  // Access the DB connection handle (needed for queries)
};

#endif
