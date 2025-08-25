#include "DatabaseManager.h"
#include "TransactionManager.h"

DatabaseManager::DatabaseManager() : hEnv(NULL), hDbc(NULL), ret(SQL_SUCCESS) {}

DatabaseManager::~DatabaseManager() {
    disconnect(); // Ensure cleanup on destruction
}

bool DatabaseManager::connect() {

    // Allocate environment handle:::::::::::::::::::::::::::::
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating environment handle\n";
        return false;
    }

    // Set the ODBC version environment attribute
    ret = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error setting ODBC version\n";
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        return false;
    }

    // Allocate connection handle
    ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating connection handle\n";
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        return false;
    }


    // Trusted Connection string
    SQLCHAR connStr[] = 
        "DRIVER={ODBC Driver 17 for SQL Server};"
        "SERVER=PSILENL082;"
        "DATABASE=LibraryDB;"
        "Trusted_Connection=Yes;";

    // Try connecting
    ret = SQLDriverConnect(hDbc, NULL, connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    if(SQL_SUCCEEDED(ret)) {
        std::cout << "Connected to SQL Server successfully!\n";
        return true;
    } else {
        std::cerr << "Failed to connect to SQL Server.\n";
        printError("SQLDriverConnect", hDbc, SQL_HANDLE_DBC);
        disconnect(); // Clean up handles
        return false;
    }
}


void DatabaseManager::disconnect() {
    if(hDbc){
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        hDbc = NULL;
    }
    if(hEnv){
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        hEnv = NULL;
    }
}


bool DatabaseManager::isConnected() {
    return hDbc != NULL;
}


void DatabaseManager::printError(const std::string& fn, SQLHANDLE handle, SQLSMALLINT type) {
    SQLINTEGER i = 0;
    SQLINTEGER native;
    SQLCHAR state[7];
    SQLCHAR text[256];
    SQLSMALLINT len;
    SQLRETURN ret;

    std::cout << "Diagnostics from " << fn << ":\n";

    do {
        ret = SQLGetDiagRec(type, handle, ++i, state, &native, text, sizeof(text), &len);
        if (SQL_SUCCEEDED(ret))
            std::cout << " " << state << " | Native Error: " << native 
                      << " | Message: " << text << "\n";
    } while (ret == SQL_SUCCESS);
}

SQLHDBC DatabaseManager::getDbc() {
    return hDbc;
}
