#pragma once
#include "DatabaseManager.h"
#include "BookManager.h"

class MemberManager {
public:
    static void addMember(DatabaseManager& db);
    static void updateMember(DatabaseManager& db);
    static void deleteMember(DatabaseManager& db);
    static void viewMembers(DatabaseManager& db);
    static void searchMembers(DatabaseManager& db);
};
