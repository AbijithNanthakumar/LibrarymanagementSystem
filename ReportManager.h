// ReportManager.h
#ifndef REPORT_MANAGER_H
#define REPORT_MANAGER_H

#include "DatabaseManager.h"
#include<vector>

class ReportManager {
public:
    void viewTopIssuedBooks(DatabaseManager& db);
    void viewMostActiveMembers(DatabaseManager& db);
    void generateFineSummary(DatabaseManager& db);
    void exportReportToCSV(DatabaseManager& db);
};

#endif
