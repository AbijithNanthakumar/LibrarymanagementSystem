#include "MemberManager.h"
#include "DatabaseManager.h"
#include <iostream>
#include <string>

using namespace std;

// ========== ADD MEMBER ==========
void MemberManager::addMember(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "❌ Not connected to the database.\n";
        return;
    }

    // Input variables
    string name, email, phone, address, membershipType;

    // Get member details from user
    cin.ignore(); // Clear leftover newline
    cout << "Enter Name: ";
    getline(cin, name);
    cout << "Enter Email: ";
    getline(cin, email);
    cout << "Enter Phone: ";
    getline(cin, phone);
    cout << "Enter Address: ";
    getline(cin, address);
    cout << "Enter Membership Type (Regular/Premium): ";
    getline(cin, membershipType);

    // SQL insert query (JoinDate and Status are set by default)
    string query =
        "INSERT INTO MembersInLib "
        "(Name, Email, Phone, Address, MembershipType) "
        "VALUES (?, ?, ?, ?, ?)";

    SQLHSTMT stmt;
    SQLRETURN ret;

    // Allocate statement handle
    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (SQL_SUCCEEDED(ret)) {
        // Bind parameters
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)name.c_str(), 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)email.c_str(), 0, NULL);
        SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)phone.c_str(), 0, NULL);
        SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)address.c_str(), 0, NULL);
        SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)membershipType.c_str(), 0, NULL);

        // Execute query
        ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

        if (SQL_SUCCEEDED(ret)) {
            cout << "✅ Member added successfully!\n";
        } else {
            cerr << "❌ Failed to add member.\n";
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        // Cleanup
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}


// ========== VIEW MEMBERS ==========
void MemberManager::viewMembers(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "❌ Not connected to the database.\n";
        return;
    }

    SQLHSTMT stmt;
    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (SQL_SUCCEEDED(ret)) {
        string query = "SELECT MemberID, Name, Email, MembershipType, Status FROM MembersInLib ORDER BY MemberID";
        ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

        if (SQL_SUCCEEDED(ret)) {
            SQLINTEGER memberID;
            char name[100], email[100], membership[20], status[20];

            SQLBindCol(stmt, 1, SQL_C_SLONG, &memberID, 0, NULL);
            SQLBindCol(stmt, 2, SQL_C_CHAR, name, sizeof(name), NULL);
            SQLBindCol(stmt, 3, SQL_C_CHAR, email, sizeof(email), NULL);
            SQLBindCol(stmt, 4, SQL_C_CHAR, membership, sizeof(membership), NULL);
            SQLBindCol(stmt, 5, SQL_C_CHAR, status, sizeof(status), NULL);

            int count = 0;
            char choice;

            cout << "\n👥 List of Members (5 per page):\n";
            cout << "---------------------------------------------------------\n";
            cout << "ID\tName\t\tEmail\t\tMembership\tStatus\n";
            cout << "---------------------------------------------------------\n";

            while (SQLFetch(stmt) == SQL_SUCCESS) {
                cout << memberID << "\t" << name << "\t" << email << "\t" << membership << "\t" << status << "\n";
                count++;

                if (count % 5 == 0) {
                    cout << "\n--- [n] Next | [e] Exit --- : ";
                    cin >> choice;
                    if (choice == 'e' || choice == 'E') break;
                }
            }

        } else {
            cerr << "❌ Failed to fetch members.\n";
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}

// ========== UPDATE MEMBER ==========
void MemberManager::updateMember(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "❌ Not connected to the database.\n";
        return;
    }

    int memberId;
    string name, email, phone, address, membershipType;

    cout << "Enter Member ID to update: ";
    cin >> memberId;
    cin.ignore();

    cout << "Enter New Name: ";
    getline(cin, name);

    cout << "Enter New Email: ";
    getline(cin, email);

    cout << "Enter New Phone: ";
    getline(cin, phone);

    cout << "Enter New Address: ";
    getline(cin, address);

    cout << "Enter New Membership Type (Regular/Premium): ";
    getline(cin, membershipType);

    string query = "UPDATE MembersInLib SET Name = ?, Email = ?, Phone = ?, Address = ?, MembershipType = ? WHERE MemberID = ?";

    SQLHSTMT stmt;
    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (SQL_SUCCEEDED(ret)) {
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)name.c_str(), 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)email.c_str(), 0, NULL);
        SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)phone.c_str(), 0, NULL);
        SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLPOINTER)address.c_str(), 0, NULL);
        SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)membershipType.c_str(), 0, NULL);
        SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &memberId, 0, NULL);

        ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

        if (SQL_SUCCEEDED(ret)) {
            cout << "✅ Member updated successfully!\n";
        } else {
            cerr << "❌ Failed to update member.\n";
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}

// ========== DELETE MEMBER ==========
void MemberManager::deleteMember(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "❌ Not connected to the database.\n";
        return;
    }

    int memberId;
    cout << "Enter Member ID to delete: ";
    cin >> memberId;

    string query = "UPDATE MembersInLib SET Status = 'Inactive' WHERE MemberID = ?";

    SQLHSTMT stmt;
    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (SQL_SUCCEEDED(ret)) {
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &memberId, 0, NULL);

        ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

        if (SQL_SUCCEEDED(ret)) {
            cout << "🗑️ Member marked as inactive.\n";
        } else {
            cerr << "❌ Failed to delete member.\n";
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}

// ========== SEARCH MEMBERS ==========
void MemberManager::searchMembers(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "❌ Not connected to the database.\n";
        return;
    }

    string keyword;
    cin.ignore();
    cout << "Enter search keyword: ";
    getline(cin, keyword);

    string query = "SELECT MemberID, Name, Email, MembershipType, Status "
                   "FROM MembersInLib "
                   "WHERE Name LIKE ? OR Email LIKE ? OR MembershipType LIKE ?";

    SQLHSTMT stmt;
    SQLRETURN ret;

    
    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);

    if (SQL_SUCCEEDED(ret)) {
        string searchPattern = "%" + keyword + "%";

        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)searchPattern.c_str(), 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)searchPattern.c_str(), 0, NULL);
        SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)searchPattern.c_str(), 0, NULL);

        ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

        if (SQL_SUCCEEDED(ret)) {
            SQLINTEGER memberID;
            char name[100], email[100], membership[20], status[20];

            SQLBindCol(stmt, 1, SQL_C_SLONG, &memberID, 0, NULL);
            SQLBindCol(stmt, 2, SQL_C_CHAR, name, sizeof(name), NULL);
            SQLBindCol(stmt, 3, SQL_C_CHAR, email, sizeof(email), NULL);
            SQLBindCol(stmt, 4, SQL_C_CHAR, membership, sizeof(membership), NULL);
            SQLBindCol(stmt, 5, SQL_C_CHAR, status, sizeof(status), NULL);

            cout << "\n🔍 Search Results:\n";
            cout << "---------------------------------------------------------\n";
            cout << "ID\tName\t\tEmail\t\tMembership\tStatus\n";
            cout << "---------------------------------------------------------\n";

            while (SQLFetch(stmt) == SQL_SUCCESS) {
                cout << memberID << "\t" << name << "\t" << email << "\t" << membership << "\t" << status << "\n";
            }

        } else {
            cerr << "❌ Failed to search members.\n";
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}