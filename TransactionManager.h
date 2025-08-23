#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include "DatabaseManager.h"

class TransactionManager {
public:
    void issueBook(DatabaseManager& db);
    void returnBook(DatabaseManager& db);
    void reserveBook(DatabaseManager& db);
    void showTransactionHistory(DatabaseManager& db);
};

#endif // TRANSACTION_MANAGER_H
