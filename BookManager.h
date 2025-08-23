#ifndef BOOKMANAGER_H
#define BOOKMANAGER_H

#include "DatabaseManager.h"

class BookManager {
public:
    static void addBook(DatabaseManager& db);
    static void viewBooks(DatabaseManager& db);
    static void updateBook(DatabaseManager& db);
    static void deleteBook(DatabaseManager& db);
    static void searchBooks(DatabaseManager& db);
};

#endif
