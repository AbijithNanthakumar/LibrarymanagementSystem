#include "TransactionManager.h"
#include <iostream>
#include <string>
#include <ctime>

using namespace std;

void TransactionManager::issueBook(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "Database not connected.\n";
        return;
    }

    int memberId, bookId;
    cout << "Enter Member ID: ";
    cin >> memberId;
    cout << "Enter Book ID: ";
    cin >> bookId;

    SQLHSTMT stmt;
    SQLRETURN ret;

    // Check book availability
    string checkQuery = "SELECT Availability FROM Books WHERE BookID = ?";

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
    if (SQL_SUCCEEDED(ret)) {
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bookId, 0, NULL);
        ret = SQLExecDirect(stmt, (SQLCHAR*)checkQuery.c_str(), SQL_NTS);

        char availability[10];
        if (SQLFetch(stmt) == SQL_SUCCESS) {
            SQLGetData(stmt, 1, SQL_C_CHAR, availability, sizeof(availability), NULL);

            if (string(availability) != "Yes") {
                cerr << "Book not available.\n";
                SQLFreeHandle(SQL_HANDLE_STMT, stmt);
                return;
            }
        } else {
            cerr << "Book not found.\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return;
        }
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }

    // Insert into Transactions
    string insertQuery = "INSERT INTO Transactions (MemberID, BookID, IssueDate, DueDate) "
                         "VALUES (?, ?, GETDATE(), DATEADD(DAY, 14, GETDATE()))";

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
    if (SQL_SUCCEEDED(ret)) {
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &memberId, 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bookId, 0, NULL);

        ret = SQLExecDirect(stmt, (SQLCHAR*)insertQuery.c_str(), SQL_NTS);
        if (SQL_SUCCEEDED(ret)) {
            cout << "Book issued successfully!\n";

            // Update book availability
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
            string updateQuery = "UPDATE Books SET Availability = 'No' WHERE BookID = ?";
            SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bookId, 0, NULL);
            SQLExecDirect(stmt, (SQLCHAR*)updateQuery.c_str(), SQL_NTS);
        } else {
            cerr << "Failed to issue book.\n";
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}

void TransactionManager::returnBook(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "Database not connected.\n";
        return;
    }

    int transactionId;
    cout << "Enter Transaction ID to return book: ";
    cin >> transactionId;

    SQLHSTMT stmt;
    SQLRETURN ret;

    // Check due date
    string fetchQuery = "SELECT BookID, DueDate FROM Transactions WHERE TransactionID = ? AND ReturnDate IS NULL";
    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    int bookId;
    DATE dueDate;
    if (SQL_SUCCEEDED(ret)) {
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &transactionId, 0, NULL);
        ret = SQLExecDirect(stmt, (SQLCHAR*)fetchQuery.c_str(), SQL_NTS);
        if (SQLFetch(stmt) == SQL_SUCCESS) {
            SQLGetData(stmt, 1, SQL_C_SLONG, &bookId, 0, NULL);
            SQLGetData(stmt, 2, SQL_C_TYPE_DATE, &dueDate, 0, NULL);
        } else {
            cerr << "Transaction not found or already returned.\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return;
        }
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }

    // Update return date and fine
    string updateQuery = "UPDATE Transactions SET ReturnDate = GETDATE(), "
                         "FineAmount = CASE WHEN DATEDIFF(DAY, DueDate, GETDATE()) > 0 "
                         "THEN DATEDIFF(DAY, DueDate, GETDATE()) * 5 ELSE 0 END "
                         "WHERE TransactionID = ?";

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
    if (SQL_SUCCEEDED(ret)) {
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &transactionId, 0, NULL);
        ret = SQLExecDirect(stmt, (SQLCHAR*)updateQuery.c_str(), SQL_NTS);
        if (SQL_SUCCEEDED(ret)) {
            cout << "Book returned successfully!\n";

            // Update book availability
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            string updateBook = "UPDATE Books SET Availability = 'Yes' WHERE BookID = ?";
            ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
            SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bookId, 0, NULL);
            SQLExecDirect(stmt, (SQLCHAR*)updateBook.c_str(), SQL_NTS);
        } else {
            cerr << "Failed to return book.\n";
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}

void TransactionManager::reserveBook(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "Database not connected.\n";
        return;
    }

    int memberId, bookId;
    cout << "Enter Member ID: ";
    cin >> memberId;
    cout << "Enter Book ID to reserve: ";
    cin >> bookId;

    SQLHSTMT stmt;
    SQLRETURN ret;

    // 1. Check if book exists and is NOT available
    string checkQuery = "SELECT Availability FROM Books WHERE BookID = ?";
    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (!SQL_SUCCEEDED(ret)) {
        cerr << "Failed to allocate statement handle.\n";
        return;
    }

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bookId, 0, NULL);
    ret = SQLExecDirect(stmt, (SQLCHAR*)checkQuery.c_str(), SQL_NTS);

    char availability[10] = {0};
    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_CHAR, availability, sizeof(availability), NULL);

        if (string(availability) == "Yes") {
            cerr << "Book is currently available. No need to reserve.\n";
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return;
        }
    } else {
        cerr << "Book not found.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    // 2. Check if the member has already reserved this book
    string existsQuery = "SELECT COUNT(*) FROM Reservations WHERE MemberID = ? AND BookID = ?";
    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    SQLINTEGER count = 0;
    if (SQL_SUCCEEDED(ret)) {
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &memberId, 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bookId, 0, NULL);

        ret = SQLExecDirect(stmt, (SQLCHAR*)existsQuery.c_str(), SQL_NTS);
        if (SQLFetch(stmt) == SQL_SUCCESS) {
            SQLGetData(stmt, 1, SQL_C_SLONG, &count, 0, NULL);
        }
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }

    if (count > 0) {
        cout << "You have already reserved this book.\n";
        return;
    }

    // 3. Insert into Reservations
    string insertQuery = "INSERT INTO Reservations (MemberID, BookID, ReservationDate) VALUES (?, ?, GETDATE())";
    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (SQL_SUCCEEDED(ret)) {
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &memberId, 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bookId, 0, NULL);

        ret = SQLExecDirect(stmt, (SQLCHAR*)insertQuery.c_str(), SQL_NTS);
        if (SQL_SUCCEEDED(ret)) {
            cout << "Book reserved successfully. You will be notified when it becomes available.\n";
        } else {
            cerr << "Failed to reserve book.\n";
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}


void TransactionManager::showTransactionHistory(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "Not connected to the database.\n";
        return;
    }

    string query = 
        "SELECT t.TransactionID, t.MemberID, t.BookID, b.Title, "
        "t.IssueDate, t.DueDate, t.ReturnDate, t.FineAmount "
        "FROM Transactions t "
        "JOIN Books b ON t.BookID = b.BookID "
        "ORDER BY t.TransactionID";

    SQLHSTMT stmt;
    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (SQL_SUCCEEDED(ret)) {
        ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);
        if (SQL_SUCCEEDED(ret)) {
            int transactionId, memberId, bookId;
            char title[100];
            SQL_DATE_STRUCT issueDate, dueDate, returnDate;
            SQLLEN returnDateInd;
            float fine;

            SQLBindCol(stmt, 1, SQL_C_SLONG, &transactionId, 0, NULL);
            SQLBindCol(stmt, 2, SQL_C_SLONG, &memberId, 0, NULL);
            SQLBindCol(stmt, 3, SQL_C_SLONG, &bookId, 0, NULL);
            SQLBindCol(stmt, 4, SQL_C_CHAR, title, sizeof(title), NULL);
            SQLBindCol(stmt, 5, SQL_C_TYPE_DATE, &issueDate, 0, NULL);
            SQLBindCol(stmt, 6, SQL_C_TYPE_DATE, &dueDate, 0, NULL);
            SQLBindCol(stmt, 7, SQL_C_TYPE_DATE, &returnDate, 0, &returnDateInd);
            SQLBindCol(stmt, 8, SQL_C_FLOAT, &fine, 0, NULL);

            cout << "\nTransaction History:\n";
            cout << "--------------------------------------------------------------------------------\n";
            cout << "ID\tMemberID\tBookID\tTitle\t\tIssueDate\tDueDate\t\tReturnDate\tFine\n";
            cout << "--------------------------------------------------------------------------------\n";

            while (SQLFetch(stmt) == SQL_SUCCESS) {
                cout << transactionId << "\t" << memberId << "\t\t" << bookId << "\t" << title
                     << "\t" << issueDate.year << "-" << (issueDate.month < 10 ? "0" : "") << issueDate.month << "-" << (issueDate.day < 10 ? "0" : "") << issueDate.day
                     << "\t" << dueDate.year << "-" << (dueDate.month < 10 ? "0" : "") << dueDate.month << "-" << (dueDate.day < 10 ? "0" : "") << dueDate.day
                     << "\t";

                if (returnDateInd != SQL_NULL_DATA) {
                    cout << returnDate.year << "-" << (returnDate.month < 10 ? "0" : "") << returnDate.month << "-" << (returnDate.day < 10 ? "0" : "") << returnDate.day;
                } else {
                    cout << "N/A";
                }

                cout << "\t" << fine << "\n";
            }
        } else {
            cerr << "Failed to fetch transaction history.\n";
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    } else {
        cerr << "Failed to allocate SQL statement handle.\n";
    }
}
