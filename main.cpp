#include <iostream>
#include <cstdlib> // for system("cls")
#include "DatabaseManager.h"
#include "BookManager.h"
#include "MemberManager.h"
#include "TransactionManager.h"
#include "ReportManager.h"
using namespace std;


// // Forward declarations for modules (will be created later)
void booksMenu(DatabaseManager& db);
void membersMenu(DatabaseManager& db);
void transactionsMenu(DatabaseManager& db);
void reportsMenu(DatabaseManager& db);

void pause() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

int main() {
    DatabaseManager db;

    if (!db.connect()) {
        std::cerr << "❌ Database connection failed. Exiting." << std::endl;
        return 1;
    }

    int choice;

    do {
        system("cls"); // clear screen (Windows only)
        std::cout << "========== 📚 Library Management ==========\n";
        std::cout << "1. Books Management\n";
        std::cout << "2. Members Management\n";
        std::cout << "3. Transactions\n";
        std::cout << "4. Reports\n";
        std::cout << "5. Exit\n";
        std::cout << "===========================================\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                booksMenu(db);
                break;
            case 2:
                membersMenu(db);
                break;
            case 3:
                transactionsMenu(db);
                break;
            case 4:
                reportsMenu(db);
                break;
            case 5:
                std::cout << "👋 Exiting the application. Goodbye!\n";
                break;
            default:
                std::cout << "❌ Invalid choice. Try again.\n";
        }

        if (choice != 5) pause();

    } while (choice != 5);

    db.disconnect();
    return 0;
}

void booksMenu(DatabaseManager& db) {
    int choice;
    do {
        // system("cls");  // Clear screen for a clean UI
        std::cout << "========== 📚 Books Management ==========\n";
        std::cout << "1. Add Book\n";
        std::cout << "2. View Books\n";
        std::cout << "3. Update Book\n";
        std::cout << "4. Delete Book\n"; 
        std::cout << "5. Search Books\n";
        std::cout << "6. Bulk Import from CSV\n";
        std::cout << "7. Back to Main Menu\n";
        std::cout << "=========================================\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                BookManager::addBook(db);
                break;
            case 2:
                BookManager::viewBooks(db);
                break;
            case 3:
                BookManager::updateBook(db);
                break;
            case 4:
                BookManager::deleteBook(db);  // ✅ Call deleteBook method
                break;
            case 5: 
                BookManager::searchBooks(db);
                break;
            case 6:
                 BookManager::bulkImportBooks(db);
                 break;
            case 7:
                std::cout << "Returning to Main Menu...\n";
                break;
            default:
                std::cout << "❌ Invalid choice. Try again.\n";
        }

        if (choice != 5) {
            std::cin.ignore();
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
        }

    } while (choice != 5);
}


void membersMenu(DatabaseManager& db) {
    MemberManager memberManager;
    int choice;

    do {
        cout << "\n========== Members Management ==========\n";
        cout << "1. Add Member\n";
        cout << "2. Update Member\n";
        cout << "3. Delete Member\n";
        cout << "4. View Members\n";
        cout << "5. Search Members\n";
        cout << "6. Back to Main Menu\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                memberManager.addMember(db);
                break;
            case 2:
                memberManager.updateMember(db);
                break;
            case 3:
                memberManager.deleteMember(db);
                break;
            case 4:
                memberManager.viewMembers(db);
                break;
            case 5:
                memberManager.searchMembers(db);
                break;
            case 6:
                cout << "🔙 Returning to Main Menu...\n";
                break;
            default:
                cout << "❗ Invalid choice. Please try again.\n";
        }

    } while (choice != 6);
}

void transactionsMenu(DatabaseManager& db) {
    TransactionManager transactionManager;
    int choice;

    do {
        cout << "\n========== Transactions Management ==========\n";
        cout << "1. Issue Book\n";
        cout << "2. Return Book\n";
        cout << "3. Reserve Book\n";
        cout << "4. View Transaction History\n";
        cout << "5. Back to Main Menu\n";
        cout << "=============================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                transactionManager.issueBook(db);
                break;
            case 2:
                transactionManager.returnBook(db);
                break;
            case 3:
                transactionManager.reserveBook(db);
                break;
            case 4:
                transactionManager.showTransactionHistory(db);
                break;
            case 5:
                cout << "🔙 Returning to Main Menu...\n";
                break;
            default:
                cout << "❗ Invalid choice. Please try again.\n";
        }

    } while (choice != 5);
}


void reportsMenu(DatabaseManager& db) {
    ReportManager reportManager;
    int choice;

    do {
        cout << "\n========== Reports & Analytics ==========\n";
        cout << "1. View Top 10 Issued Books\n";
        cout << "2. View Most Active Members\n";
        cout << "3. Fine Collection Summary\n";
        cout << "4. Export Top Books to CSV\n";
        cout << "5. Back to Main Menu\n";
        cout << "=========================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                reportManager.viewTopIssuedBooks(db);
                break;
            case 2:
                reportManager.viewMostActiveMembers(db);
                break;
            case 3:
                reportManager.generateFineSummary(db);
                break;
            case 4:
                reportManager.exportReportToCSV(db);
                break;
            case 5:
                cout << "🔙 Returning to Main Menu...\n";
                break;
            default:
                cout << "❗ Invalid choice. Please try again.\n";
        }

    } while (choice != 5);
}


