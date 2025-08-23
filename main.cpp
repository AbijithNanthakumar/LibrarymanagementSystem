#include <iostream>
#include <cstdlib> // for system("cls")
#include "DatabaseManager.h"
#include "BookManager.h"


// Forward declarations for modules (will be created later)
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
        system("cls");  // Clear screen for a clean UI
        std::cout << "========== 📚 Books Management ==========\n";
        std::cout << "1. Add Book\n";
        std::cout << "2. View Books\n";
        std::cout << "3. Update Book\n";
        std::cout << "4. Delete Book\n";  // ✅ New option added
        std::cout << "5. Back to Main Menu\n";
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
    std::cout << "👤 Members menu - (To be implemented)\n";
}

void transactionsMenu(DatabaseManager& db) {
    std::cout << "🔁 Transactions menu - (To be implemented)\n";
}

void reportsMenu(DatabaseManager& db) {
    std::cout << "📊 Reports menu - (To be implemented)\n";
}

