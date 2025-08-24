#include "BookManager.h"  // Include the header for class definition
#include <string>
#include <iostream>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include "DatabaseManager.h"

using namespace std;

// ====================
// Function: addBook()
// Purpose: Add a new book to the database with user input
// ====================
void BookManager::addBook(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "Not connected to database.\n";
        return;
    }

    // Input variables
    string title, author, genre, publisher, isbn, edition, rackLocation, language;
    int publishedYear;
    float price;

    // Get book details from user
    cin.ignore(); // Clear any leftover newline
    cout << "Enter Title: ";
    getline(cin, title);
    cout << "Enter Author: ";
    getline(cin, author);
    cout << "Enter Genre: ";
    getline(cin, genre);
    cout << "Enter Publisher: ";
    getline(cin, publisher);
    cout << "Enter ISBN (Unique): ";
    getline(cin, isbn);
    cout << "Enter Edition: ";
    getline(cin, edition);
    cout << "Enter Published Year: ";
    cin >> publishedYear;
    cout << "Enter Price: ";
    cin >> price;
    cin.ignore();
    cout << "Enter Rack Location: ";
    getline(cin, rackLocation);
    cout << "Enter Language: ";
    getline(cin, language);

    // SQL insert query with placeholders (?)
    string query =
        "INSERT INTO Books "
        "(Title, Author, Genre, Publisher, ISBN, Edition, PublishedYear, Price, RackLocation, Language, Availability) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 'Yes')";

    SQLHSTMT stmt;
    SQLRETURN ret;

    // Allocate statement handle
    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);


    if (SQL_SUCCEEDED(ret)) {
        // Bind input parameters to SQL statement
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)title.c_str(), 0, NULL);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)author.c_str(), 0, NULL);
        SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)genre.c_str(), 0, NULL);
        SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)publisher.c_str(), 0, NULL);
        SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)isbn.c_str(), 0, NULL);
        SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)edition.c_str(), 0, NULL);
        SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &publishedYear, 0, NULL);
        SQLBindParameter(stmt, 8, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, &price, 0, NULL);
        SQLBindParameter(stmt, 9, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)rackLocation.c_str(), 0, NULL);
        SQLBindParameter(stmt, 10,SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)language.c_str(), 0, NULL);

        // Execute the statement
        ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

        if (SQL_SUCCEEDED(ret)) {
            cout << "Book added successfully!\n";
        } else {
            cerr << "Failed to add book.\n";
            db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
        }

        // Clean up
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}


// ==========================
// Function: viewBooks()
// Purpose: Show books in pages of 5 at a time (pagination)
// ==========================
void BookManager::viewBooks(DatabaseManager& db) {
    if (!db.isConnected()) {
        std::cerr << "Not connected to database.\n";
        return;
    }

    SQLHSTMT stmt;
    SQLRETURN ret;

    // Allocate statement handle
    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Failed to allocate statement handle.\n";
        return;
    }

    // Pagination variables
    int pageSize = 5;    // Number of records per page
    int pageNum = 0;     // Current page number (starting from 0)
    char choice;

    while (true) {
        // Calculate OFFSET for SQL query
        int offset = pageNum * pageSize;

        // SQL query with OFFSET-FETCH for pagination
        std::string query = 
            "SELECT BookID, Title, Author, ISBN FROM Books "
            "ORDER BY BookID "
            "OFFSET ? ROWS FETCH NEXT ? ROWS ONLY";

        // Prepare the statement
        ret = SQLPrepare(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "Failed to prepare SQL statement.\n";
            db.printError("SQLPrepare", stmt, SQL_HANDLE_STMT);
            break;
        }

        // Bind OFFSET parameter (first ?) // how many it can skip and display 
        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &offset, 0, NULL);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "Failed to bind OFFSET parameter.\n";
            db.printError("SQLBindParameter", stmt, SQL_HANDLE_STMT);
            break;
        }

        // Bind FETCH NEXT parameter (second ?) // how many row to fetch
        ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &pageSize, 0, NULL);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "Failed to bind FETCH NEXT parameter.\n";
            db.printError("SQLBindParameter", stmt, SQL_HANDLE_STMT);
            break;
        }

        // Execute the query
        ret = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret)) {
            std::cerr << "Failed to execute SQL statement.\n";
            db.printError("SQLExecute", stmt, SQL_HANDLE_STMT);
            break;
        }

        // Variables to hold fetched columns
        SQLINTEGER bookID;
        char title[100] = {0};
        char author[100] = {0};
        char isbn[20] = {0};

        // Bind columns to variables // one variable value at a time 
        SQLBindCol(stmt, 1, SQL_C_SLONG, &bookID, 0, NULL);
        SQLBindCol(stmt, 2, SQL_C_CHAR, title, sizeof(title), NULL);
        SQLBindCol(stmt, 3, SQL_C_CHAR, author, sizeof(author), NULL);
        SQLBindCol(stmt, 4, SQL_C_CHAR, isbn, sizeof(isbn), NULL);

        std::cout << "\nList of Books (Page " << (pageNum + 1) << "):\n";
        std::cout << "--------------------------------------------------\n";
        std::cout << "ID\tTitle\t\tAuthor\t\tISBN\n";
        std::cout << "--------------------------------------------------\n";

        int rowCount = 0;

        // Fetch and display all rows on this page
        while (SQLFetch(stmt) == SQL_SUCCESS) {
            std::cout << bookID << "\t" << title << "\t" << author << "\t" << isbn << "\n";
            rowCount++;
        }


        // If no rows returned, means no more data
        if (rowCount == 0) {
            std::cout << "No more books to display.\n";
            break;
        }

        // Ask user what to do next
        std::cout << "\n--- [n] Next Page | [p] Previous Page | [e] Exit --- : ";
        std::cin >> choice;

        // Clear the statement for next use
        SQLFreeStmt(stmt, SQL_CLOSE);

        if (choice == 'n' || choice == 'N') {
            pageNum++;  // Next page
        } else if ((choice == 'p' || choice == 'P') && pageNum > 0) {
            pageNum--;  // Previous page
        } else if (choice == 'e' || choice == 'E') {
            break;      // Exit viewing
        } else {
            std::cout << "Invalid choice, exiting.\n";
            break;
        }
    }

    // Cleanup
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


// UpdateBook
void BookManager::updateBook(DatabaseManager& db) {
    if (!db.isConnected()) {
        std::cerr << "Not connected to database.\n";
        return;
    }

    int bookID;
    std::cout << "Enter Book ID to update: ";
    std::cin >> bookID;
    std::cin.ignore(); // clear newline

    // Step 1: Fetch current book details
    SQLHSTMT stmt;
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle.\n";
        return;
    }

    std::string selectQuery = "SELECT Title, Author, Genre, Publisher, ISBN, Edition, PublishedYear, Price, RackLocation, Language FROM Books WHERE BookID = ?";
    ret = SQLPrepare(stmt, (SQLCHAR*)selectQuery.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Failed to prepare select statement.\n";
        db.printError("SQLPrepare", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bookID, 0, NULL);
    ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Failed to execute select statement.\n";
        db.printError("SQLExecute", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    char title[101], author[101], genre[101], publisher[101], isbn[21], edition[51], rackLocation[51], language[51];
    SQLINTEGER publishedYear;
    float price;

    // Bind columns
    SQLBindCol(stmt, 1, SQL_C_CHAR, title, sizeof(title), NULL);
    SQLBindCol(stmt, 2, SQL_C_CHAR, author, sizeof(author), NULL);
    SQLBindCol(stmt, 3, SQL_C_CHAR, genre, sizeof(genre), NULL);
    SQLBindCol(stmt, 4, SQL_C_CHAR, publisher, sizeof(publisher), NULL);
    SQLBindCol(stmt, 5, SQL_C_CHAR, isbn, sizeof(isbn), NULL);
    SQLBindCol(stmt, 6, SQL_C_CHAR, edition, sizeof(edition), NULL);
    SQLBindCol(stmt, 7, SQL_C_SLONG, &publishedYear, 0, NULL);
    SQLBindCol(stmt, 8, SQL_C_FLOAT, &price, 0, NULL);
    SQLBindCol(stmt, 9, SQL_C_CHAR, rackLocation, sizeof(rackLocation), NULL);
    SQLBindCol(stmt, 10, SQL_C_CHAR, language, sizeof(language), NULL);

    ret = SQLFetch(stmt);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Book ID not found.\n";
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    std::cout << "\nCurrent Book Details:\n";
    std::cout << "Title: " << title << "\nAuthor: " << author << "\nGenre: " << genre << "\nPublisher: " << publisher
              << "\nISBN: " << isbn << "\nEdition: " << edition << "\nPublished Year: " << publishedYear
              << "\nPrice: " << price << "\nRack Location: " << rackLocation << "\nLanguage: " << language << "\n";

    // Step 2: Get new details (press Enter to keep old)
    std::string input;

    std::cout << "\nEnter new Title (leave empty to keep current): ";
    std::getline(std::cin, input);
    if (!input.empty()) strncpy_s(title, input.c_str(), sizeof(title) - 1);

    std::cout << "Enter new Author (leave empty to keep current): ";
    std::getline(std::cin, input);
    if (!input.empty()) strncpy_s(author, input.c_str(), sizeof(author) - 1);

    std::cout << "Enter new Genre (leave empty to keep current): ";
    std::getline(std::cin, input);
    if (!input.empty()) strncpy_s(genre, input.c_str(), sizeof(genre) - 1);

    std::cout << "Enter new Publisher (leave empty to keep current): ";
    std::getline(std::cin, input);
    if (!input.empty()) strncpy_s(publisher, input.c_str(), sizeof(publisher) - 1);

    std::cout << "Enter new ISBN (leave empty to keep current): ";
    std::getline(std::cin, input);
    if (!input.empty()) {
        // TODO: Validate ISBN uniqueness here before updating
        strncpy_s(isbn, input.c_str(), sizeof(isbn) - 1);
    }

    std::cout << "Enter new Edition (leave empty to keep current): ";
    std::getline(std::cin, input);
    if (!input.empty()) strncpy_s(edition, input.c_str(), sizeof(edition) - 1);

    std::cout << "Enter new Published Year (leave empty to keep current): ";
    std::getline(std::cin, input);
    if (!input.empty()) publishedYear = std::stoi(input);

    std::cout << "Enter new Price (leave empty to keep current): ";
    std::getline(std::cin, input);
    if (!input.empty()) price = std::stof(input);

    std::cout << "Enter new Rack Location (leave empty to keep current): ";
    std::getline(std::cin, input);
    if (!input.empty()) strncpy_s(rackLocation, input.c_str(), sizeof(rackLocation) - 1);

    std::cout << "Enter new Language (leave empty to keep current): ";
    std::getline(std::cin, input);
    if (!input.empty()) strncpy_s(language, input.c_str(), sizeof(language) - 1);

    // Step 3: Update the database record
    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Error allocating statement handle for update.\n";
        return;
    }

    std::string updateQuery =
        "UPDATE Books SET Title = ?, Author = ?, Genre = ?, Publisher = ?, ISBN = ?, Edition = ?, "
        "PublishedYear = ?, Price = ?, RackLocation = ?, Language = ? WHERE BookID = ?";

    ret = SQLPrepare(stmt, (SQLCHAR*)updateQuery.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        std::cerr << "Failed to prepare update statement.\n";
        db.printError("SQLPrepare", stmt, SQL_HANDLE_STMT);
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return;
    }

    // Bind parameters
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, title, 0, NULL);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, author, 0, NULL);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, genre, 0, NULL);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, publisher, 0, NULL);
    SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, isbn, 0, NULL);
    SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, edition, 0, NULL);
    SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &publishedYear, 0, NULL);
    SQLBindParameter(stmt, 8, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, &price, 0, NULL);
    SQLBindParameter(stmt, 9, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, rackLocation, 0, NULL);
    SQLBindParameter(stmt, 10, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, language, 0, NULL);
    SQLBindParameter(stmt, 11, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bookID, 0, NULL);

    ret = SQLExecute(stmt);
    if (SQL_SUCCEEDED(ret)) {
        std::cout << "Book updated successfully!\n";
    } else {
        std::cerr << "Failed to update book.\n";
        db.printError("SQLExecute", stmt, SQL_HANDLE_STMT);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


// ====================
// Function: deleteBook()
// Purpose: Delete a book only if it is not issued or reserved
// ====================
void BookManager::deleteBook(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "Not connected to database.\n";
        return;
    }

    int bookID;
    cout << "Enter BookID to delete: ";
    cin >> bookID;

    SQLHSTMT stmt;
    SQLRETURN ret;

    // Step 1: Check if the book exists
    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
    if (SQL_SUCCEEDED(ret)) {
        string checkQuery = "SELECT COUNT(*) FROM Books WHERE BookID = ?";
        SQLPrepare(stmt, (SQLCHAR*)checkQuery.c_str(), SQL_NTS);
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bookID, 0, NULL);

        SQLINTEGER count = 0;
        SQLBindCol(stmt, 1, SQL_C_SLONG, &count, 0, NULL);
        ret = SQLExecute(stmt);
        SQLFetch(stmt);

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        if (count == 0) {
            cout << "BookID not found.\n";
            return;
        }
    }

    // Step 2: Check if the book is issued or reserved
    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
    if (SQL_SUCCEEDED(ret)) {
        string checkIssued = "SELECT COUNT(*) FROM Transactions WHERE BookID = ? AND ReturnDate IS NULL";
        SQLPrepare(stmt, (SQLCHAR*)checkIssued.c_str(), SQL_NTS);
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bookID, 0, NULL);

        SQLINTEGER count = 0;
        SQLBindCol(stmt, 1, SQL_C_SLONG, &count, 0, NULL);
        ret = SQLExecute(stmt);
        SQLFetch(stmt);

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        if (count > 0) {
            cout << "Cannot delete. Book is currently issued or reserved.\n";
            return;
        }
    }

    // Step 3: Delete the book
    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
    if (SQL_SUCCEEDED(ret)) {
        string deleteQuery = "DELETE FROM Books WHERE BookID = ?";
        SQLPrepare(stmt, (SQLCHAR*)deleteQuery.c_str(), SQL_NTS);
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &bookID, 0, NULL);

        ret = SQLExecute(stmt);

        if (SQL_SUCCEEDED(ret)) {
            cout << "Book deleted successfully.\n";
        } else {
            cerr << "Failed to delete book.\n";
            db.printError("SQLExecute", stmt, SQL_HANDLE_STMT);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
}


// ====================
// Function: searchBooks()
// Purpose: Search books by Title, Author, ISBN, or Genre with pagination
// ====================
void BookManager::searchBooks(DatabaseManager& db) {
    if (!db.isConnected()) {
        cerr << "Not connected to database.\n";
        return;
    }

    int option;
    string column, searchValue;

    cout << "\nSearch Books By:\n";
    cout << "1. Title\n";
    cout << "2. Author\n";
    cout << "3. ISBN\n";
    cout << "4. Genre\n";
    cout << "Choose option (1-4): ";
    cin >> option;

    switch (option) {
        case 1: column = "Title"; break;
        case 2: column = "Author"; break;
        case 3: column = "ISBN"; break;
        case 4: column = "Genre"; break;
        default:
            cout << "Invalid option.\n";
            return;
    }

    cin.ignore();
    cout << "Enter search keyword: ";
    getline(cin, searchValue);

    SQLHSTMT stmt;
    SQLRETURN ret;

    ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
    if (!SQL_SUCCEEDED(ret)) {
        cerr << "Failed to allocate statement.\n";
        return;
    }

    // Prepare dynamic SQL query with parameter
    string query = "SELECT BookID, Title, Author, ISBN FROM Books WHERE " + column + " LIKE ?";
    SQLPrepare(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

    string pattern = "%" + searchValue + "%";
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)pattern.c_str(), 0, NULL);

    ret = SQLExecute(stmt);
    if (SQL_SUCCEEDED(ret)) {
        SQLINTEGER bookID;
        char title[100], author[100], isbn[20];

        SQLBindCol(stmt, 1, SQL_C_SLONG, &bookID, 0, NULL);
        SQLBindCol(stmt, 2, SQL_C_CHAR, title, sizeof(title), NULL);
        SQLBindCol(stmt, 3, SQL_C_CHAR, author, sizeof(author), NULL);
        SQLBindCol(stmt, 4, SQL_C_CHAR, isbn, sizeof(isbn), NULL);

        cout << "\nSearch Results (5 per page):\n";
        cout << "----------------------------------------------\n";
        cout << "ID\tTitle\t\tAuthor\t\tISBN\n";
        cout << "----------------------------------------------\n";

        int count = 0;
        char next;

        while (SQLFetch(stmt) == SQL_SUCCESS) {
            cout << bookID << "\t" << title << "\t" << author << "\t" << isbn << "\n";
            count++;

            if (count % 5 == 0) {
                cout << "\n--- [n] Next | [e] Exit --- : ";
                cin >> next;
                if (next == 'e' || next == 'E') break;
            }
        }

        if (count == 0) {
            cout << "No matching records found.\n";
        }
    } else {
        cerr << "Failed to execute search query.\n";
        db.printError("SQLExecute", stmt, SQL_HANDLE_STMT);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


#include <fstream>  // For file I/O
#include <sstream>  // For string stream (CSV parsing)

// ====================
// Function: bulkImportBooks()
// Purpose: Read a CSV file and insert books into the database
// ====================
void BookManager::bulkImportBooks(DatabaseManager& db) {
    if (!db.isConnected()) {
        std::cerr << "Not connected to database.\n";
        return;
    }

    std::string filename;
    std::cout << "Enter CSV file path (e.g., books.csv): ";
    std::cin >> filename;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }

    std::string line;
    int imported = 0;
    int failed = 0;

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string title, author, genre, publisher, isbn, edition, rackLocation, language;
        int year;
        float price;

        // Parse CSV fields
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, genre, ',');
        getline(ss, publisher, ',');
        getline(ss, isbn, ',');
        getline(ss, edition, ',');
        ss >> year;
        ss.ignore(); // skip comma
        ss >> price;
        ss.ignore(); // skip comma
        getline(ss, rackLocation, ',');
        getline(ss, language);

        SQLHSTMT stmt;
        SQLRETURN ret;
        std::string query =
            "INSERT INTO Books (Title, Author, Genre, Publisher, ISBN, Edition, PublishedYear, Price, RackLocation, Language, Availability) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 'Yes')";

        ret = SQLAllocHandle(SQL_HANDLE_STMT, db.getDbc(), &stmt);
        if (SQL_SUCCEEDED(ret)) {
            // Bind parameters
            SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)title.c_str(), 0, NULL);
            SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)author.c_str(), 0, NULL);
            SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)genre.c_str(), 0, NULL);
            SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLPOINTER)publisher.c_str(), 0, NULL);
            SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)isbn.c_str(), 0, NULL);
            SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)edition.c_str(), 0, NULL);
            SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &year, 0, NULL);
            SQLBindParameter(stmt, 8, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_REAL, 0, 0, &price, 0, NULL);
            SQLBindParameter(stmt, 9, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)rackLocation.c_str(), 0, NULL);
            SQLBindParameter(stmt, 10, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLPOINTER)language.c_str(), 0, NULL);

            ret = SQLExecDirect(stmt, (SQLCHAR*)query.c_str(), SQL_NTS);

            if (SQL_SUCCEEDED(ret)) {
                imported++;
            } else {
                failed++;
                std::cerr << "Failed to insert: " << title << "\n";
                db.printError("SQLExecDirect", stmt, SQL_HANDLE_STMT);
            }

            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        }
    }

    file.close();
    std::cout << "Import complete. Imported: " << imported << ", Failed: " << failed << "\n";
}
