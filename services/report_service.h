// services/report_service.h
#ifndef REPORT_SERVICE_H
#define REPORT_SERVICE_H

#include <string>
#include <vector>
#include <map>
#include "../database/db_manager.h"

struct ClientActivityReport {
    int clientId;
    std::string clientName;
    int totalClasses;
    int cancelledClasses;
    std::string mostFrequentClass;
    std::string lastVisit;
};

struct ClassPopularityReport {
    int classId;
    std::string className;
    std::string trainer;
    int totalReservations;
    double fillRate; // procentowo
};

struct FinancialReport {
    double totalRevenue;
    int totalMemberships;
    double studentRevenue;
    double standardRevenue;
    std::map<std::string, double> revenueByMembershipType;
    std::map<std::string, int> membershipCountByType;
};

class ReportService {
public:
    explicit ReportService(DBManager& dbManager);

    // Raporty aktywnoœci klientów
    std::vector<ClientActivityReport> getClientActivityReport(const std::string& startDate, const std::string& endDate);
    ClientActivityReport getClientActivityReport(int clientId, const std::string& startDate, const std::string& endDate);

    // Raporty popularnoœci zajêæ
    std::vector<ClassPopularityReport> getClassPopularityReport(const std::string& startDate, const std::string& endDate);
    ClassPopularityReport getClassPopularityReport(int classId, const std::string& startDate, const std::string& endDate);

    // Raporty finansowe
    FinancialReport getFinancialReport(const std::string& startDate, const std::string& endDate);

private:
    DBManager& dbManager;
};

#endif // REPORT_SERVICE_H