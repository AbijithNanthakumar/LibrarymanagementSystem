#include "UserManager.h"      // Your header where class and login() declared
#include "DatabaseManager.h"  // Your DB manager to get connection handle
#include <iostream>
#include <string>
#include <limits>

using namespace std;

bool UserManager::login(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "Not connected to the database.\n";
        return false;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string username, password;
    cin.ignore(); // clear input buffer
    cout << "Enter Username: \n";
    getline(cin, username);
    cout << "Enter Password: \n";
    getline(cin, password);

    string query = "SELECT COUNT(*) FROM Users WHERE username = ? AND password = ?";

    SQLHSTMT stmt;
    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (SQL_SUCCEEDED(ret)) {
        // Bind parameters: username and password
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, username.size(), 0,
                         (SQLPOINTER)username.c_str(), 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, password.size(), 0,
                         (SQLPOINTER)password.c_str(), 0, NULL);

        ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

        if (SQL_SUCCEEDED(ret)) {
            SQLINTEGER count = 0;
            SQLBindCol(stmt, 1, SQL_C_SLONG, &count, 0, NULL);

            ret = SQLFetch(stmt);
            if (SQL_SUCCEEDED(ret) && count > 0) {
                cout << "Login successful!\n";
                SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                return true;
            } else {
                cout << "Invalid username or password.\n";
            }
        } else {
            cerr << "Login query execution failed.\n";
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    } else {
        cerr << "Failed to allocate statement handle.\n";
    }

    return false;
}
