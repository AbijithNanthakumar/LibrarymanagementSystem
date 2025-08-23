// ReportManager.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include "DatabaseManager.h"
#include "BookManager.h"
#include "MemberManager.h"
#include "TransactionManager.h"
#include "ReportManager.h"

using namespace std;

void ReportManager::viewTopIssuedBooks(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "❌ Not connected to database.\n";
        return;
    }

    SQLHSTMT stmt;
    SQLRETURN ret;

    string query =
        "SELECT TOP 10 b.Title, COUNT(t.TransactionID) AS TimesIssued "
        "FROM Transactions t "
        "JOIN Books b ON t.BookID = b.BookID "
        "GROUP BY b.Title "
        "ORDER BY TimesIssued DESC";

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (SQL_SUCCEEDED(ret)) {
        ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

        if (SQL_SUCCEEDED(ret)) {
            char title[100];
            SQLINTEGER count;

            SQLBindCol(stmt, 1, SQL_C_CHAR, title, sizeof(title), NULL);
            SQLBindCol(stmt, 2, SQL_C_SLONG, &count, 0, NULL);

            cout << "\n📚 Top 10 Issued Books:\n";
            cout << "-------------------------------------\n";
            cout << "Title\t\tTimes Issued\n";
            cout << "-------------------------------------\n";

            while (SQLFetch(stmt) == SQL_SUCCESS) {
                cout << title << "\t\t" << count << endl;
            }

        } else {
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}


// Most users:::
void ReportManager::viewMostActiveMembers(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "❌ Not connected to database.\n";
        return;
    }

    SQLHSTMT stmt;
    SQLRETURN ret;

    string query =
        "SELECT TOP 10 m.Name, COUNT(t.TransactionID) AS Transactions "
        "FROM Transactions t "
        "JOIN MembersInLib m ON t.MemberID = m.MemberID "
        "GROUP BY m.Name "
        "ORDER BY Transactions DESC";

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (SQL_SUCCEEDED(ret)) {
        ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

        if (SQL_SUCCEEDED(ret)) {
            char name[100];
            SQLINTEGER transactionCount;

            SQLBindCol(stmt, 1, SQL_C_CHAR, name, sizeof(name), NULL);
            SQLBindCol(stmt, 2, SQL_C_SLONG, &transactionCount, 0, NULL);

            cout << "\n👥 Most Active Members:\n";
            cout << "------------------------------\n";
            cout << "Name\t\tTransactions\n";
            cout << "------------------------------\n";

            while (SQLFetch(stmt) == SQL_SUCCESS) {
                cout << name << "\t\t" << transactionCount << endl;
            }

        } else {
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}


void ReportManager::generateFineSummary(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "❌ Not connected to database.\n";
        return;
    }

    SQLHSTMT stmt;
    SQLRETURN ret;

    string query =
        "SELECT SUM(FineAmount) AS TotalFines "
        "FROM Transactions "
        "WHERE FineAmount IS NOT NULL";

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (SQL_SUCCEEDED(ret)) {
        ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

        if (SQL_SUCCEEDED(ret)) {
            float totalFines = 0.0f;
            SQLBindCol(stmt, 1, SQL_C_FLOAT, &totalFines, 0, NULL);

            if (SQLFetch(stmt) == SQL_SUCCESS) {
                cout << "\n💰 Fine Collection Summary:\n";
                cout << "--------------------------\n";
                cout << "Total Fines Collected: ₹ " << totalFines << "\n";
            }

        } else {
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}

void ReportManager::exportReportToCSV(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "❌ Not connected to database.\n";
        return;
    }

    string filename;
    cout << "Enter filename to export (e.g., top_books.csv): ";
    cin.ignore();
    getline(cin, filename);

    SQLHSTMT stmt;
    SQLRETURN ret;

    string query =
        "SELECT TOP 10 B.Title, COUNT(T.BookID) AS TimesIssued "
        "FROM Transactions T "
        "JOIN Books B ON T.BookID = B.BookID "
        "GROUP BY B.Title "
        "ORDER BY TimesIssued DESC";

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (!SQL_SUCCEEDED(ret)) {
        cerr << "❌ Failed to allocate SQL handle.\n";
        return;
    }

    ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    if (!SQL_SUCCEEDED(ret)) {
        db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    char title[100];
    SQLINTEGER timesIssued;

    SQLBindCol(stmt, 1, SQL_C_CHAR, title, sizeof(title), NULL);
    SQLBindCol(stmt, 2, SQL_C_SLONG, &timesIssued, 0, NULL);

    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "❌ Failed to create file.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    outFile << "Title,Times Issued\n";

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        outFile << "\"" << title << "\"," << timesIssued << "\n";
    }

    outFile.close();
    cout << "✅ Data exported successfully to " << filename << "\n";

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}
