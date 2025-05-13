// services/report_service.cpp
#include "report_service.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <algorithm>

ReportService::ReportService(DBManager& dbManager) : dbManager(dbManager) {
}

std::vector<ClientActivityReport> ReportService::getClientActivityReport(const std::string& startDate, const std::string& endDate) {
    std::vector<ClientActivityReport> reports;

    sqlite3* db = dbManager.getConnection();

    // Zapytanie pobieraj¹ce aktywnoœæ klientów w podanym okresie
    std::string query =
        "SELECT c.id, c.first_name || ' ' || c.last_name as client_name, "
        "COUNT(r.id) as total_classes, "
        "SUM(CASE WHEN r.status = 'cancelled' THEN 1 ELSE 0 END) as cancelled_classes, "
        "(SELECT cl.name FROM classes cl "
        "JOIN reservations r2 ON cl.id = r2.class_id "
        "WHERE r2.client_id = c.id AND r2.status = 'confirmed' "
        "AND date(r2.reservation_date) BETWEEN date(?) AND date(?) "
        "GROUP BY cl.name ORDER BY COUNT(*) DESC LIMIT 1) as most_frequent_class, "
        "MAX(r.reservation_date) as last_visit "
        "FROM clients c "
        "LEFT JOIN reservations r ON c.id = r.client_id "
        "LEFT JOIN classes cl ON r.class_id = cl.id "
        "WHERE date(r.reservation_date) BETWEEN date(?) AND date(?) "
        "GROUP BY c.id "
        "ORDER BY total_classes DESC";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, startDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, endDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, startDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, endDate.c_str(), -1, SQLITE_STATIC);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        ClientActivityReport report;

        report.clientId = sqlite3_column_int(stmt, 0);

        const char* clientName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        report.clientName = clientName ? std::string(clientName) : "";

        report.totalClasses = sqlite3_column_int(stmt, 2);
        report.cancelledClasses = sqlite3_column_int(stmt, 3);

        const char* mostFrequentClass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        report.mostFrequentClass = mostFrequentClass ? std::string(mostFrequentClass) : "Brak danych";

        const char* lastVisit = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        report.lastVisit = lastVisit ? std::string(lastVisit) : "Brak danych";

        reports.push_back(report);
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return reports;
}

ClientActivityReport ReportService::getClientActivityReport(int clientId, const std::string& startDate, const std::string& endDate) {
    ClientActivityReport report;
    report.clientId = clientId;

    sqlite3* db = dbManager.getConnection();

    // Zapytanie pobieraj¹ce aktywnoœæ konkretnego klienta w podanym okresie
    std::string query =
        "SELECT c.first_name || ' ' || c.last_name as client_name, "
        "COUNT(r.id) as total_classes, "
        "SUM(CASE WHEN r.status = 'cancelled' THEN 1 ELSE 0 END) as cancelled_classes, "
        "(SELECT cl.name FROM classes cl "
        "JOIN reservations r2 ON cl.id = r2.class_id "
        "WHERE r2.client_id = c.id AND r2.status = 'confirmed' "
        "AND date(r2.reservation_date) BETWEEN date(?) AND date(?) "
        "GROUP BY cl.name ORDER BY COUNT(*) DESC LIMIT 1) as most_frequent_class, "
        "MAX(r.reservation_date) as last_visit "
        "FROM clients c "
        "LEFT JOIN reservations r ON c.id = r.client_id "
        "LEFT JOIN classes cl ON r.class_id = cl.id "
        "WHERE c.id = ? AND date(r.reservation_date) BETWEEN date(?) AND date(?) "
        "GROUP BY c.id";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, startDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, endDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, clientId);
    sqlite3_bind_text(stmt, 4, startDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, endDate.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        const char* clientName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        report.clientName = clientName ? std::string(clientName) : "";

        report.totalClasses = sqlite3_column_int(stmt, 1);
        report.cancelledClasses = sqlite3_column_int(stmt, 2);

        const char* mostFrequentClass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        report.mostFrequentClass = mostFrequentClass ? std::string(mostFrequentClass) : "Brak danych";

        const char* lastVisit = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        report.lastVisit = lastVisit ? std::string(lastVisit) : "Brak danych";
    }
    else {
        // Pobieranie samego imienia i nazwiska klienta, jeœli nie ma aktywnoœci
        std::string nameQuery = "SELECT first_name || ' ' || last_name FROM clients WHERE id = ?";
        sqlite3_stmt* nameStmt;
        int nameRc = sqlite3_prepare_v2(db, nameQuery.c_str(), -1, &nameStmt, nullptr);

        if (nameRc == SQLITE_OK) {
            sqlite3_bind_int(nameStmt, 1, clientId);

            if (sqlite3_step(nameStmt) == SQLITE_ROW) {
                const char* clientName = reinterpret_cast<const char*>(sqlite3_column_text(nameStmt, 0));
                report.clientName = clientName ? std::string(clientName) : "";
            }

            sqlite3_finalize(nameStmt);
        }

        report.totalClasses = 0;
        report.cancelledClasses = 0;
        report.mostFrequentClass = "Brak danych";
        report.lastVisit = "Brak danych";
    }

    sqlite3_finalize(stmt);

    return report;
}

std::vector<ClassPopularityReport> ReportService::getClassPopularityReport(const std::string& startDate, const std::string& endDate) {
    std::vector<ClassPopularityReport> reports;

    sqlite3* db = dbManager.getConnection();

    // Zapytanie pobieraj¹ce popularnoœæ zajêæ w podanym okresie
    std::string query =
        "SELECT cl.id, cl.name, cl.trainer, "
        "COUNT(r.id) as total_reservations, "
        "CAST(COUNT(r.id) AS REAL) / cl.max_participants * 100 as fill_rate "
        "FROM classes cl "
        "LEFT JOIN reservations r ON cl.id = r.class_id "
        "WHERE r.status = 'confirmed' AND date(cl.date) BETWEEN date(?) AND date(?) "
        "GROUP BY cl.id "
        "ORDER BY total_reservations DESC";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, startDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, endDate.c_str(), -1, SQLITE_STATIC);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        ClassPopularityReport report;

        report.classId = sqlite3_column_int(stmt, 0);

        const char* className = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        report.className = className ? std::string(className) : "";

        const char* trainer = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        report.trainer = trainer ? std::string(trainer) : "";

        report.totalReservations = sqlite3_column_int(stmt, 3);
        report.fillRate = sqlite3_column_double(stmt, 4);

        reports.push_back(report);
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return reports;
}

ClassPopularityReport ReportService::getClassPopularityReport(int classId, const std::string& startDate, const std::string& endDate) {
    ClassPopularityReport report;
    report.classId = classId;

    sqlite3* db = dbManager.getConnection();

    // Zapytanie pobieraj¹ce popularnoœæ konkretnych zajêæ w podanym okresie
    std::string query =
        "SELECT cl.name, cl.trainer, "
        "COUNT(r.id) as total_reservations, "
        "CAST(COUNT(r.id) AS REAL) / cl.max_participants * 100 as fill_rate "
        "FROM classes cl "
        "LEFT JOIN reservations r ON cl.id = r.class_id "
        "WHERE cl.id = ? AND r.status = 'confirmed' AND date(cl.date) BETWEEN date(?) AND date(?) "
        "GROUP BY cl.id";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, classId);
    sqlite3_bind_text(stmt, 2, startDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, endDate.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        const char* className = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        report.className = className ? std::string(className) : "";

        const char* trainer = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        report.trainer = trainer ? std::string(trainer) : "";

        report.totalReservations = sqlite3_column_int(stmt, 2);
        report.fillRate = sqlite3_column_double(stmt, 3);
    }
    else {
        // Pobieranie podstawowych informacji o zajêciach, jeœli nie ma rezerwacji
        std::string classQuery = "SELECT name, trainer FROM classes WHERE id = ?";
        sqlite3_stmt* classStmt;
        int classRc = sqlite3_prepare_v2(db, classQuery.c_str(), -1, &classStmt, nullptr);

        if (classRc == SQLITE_OK) {
            sqlite3_bind_int(classStmt, 1, classId);

            if (sqlite3_step(classStmt) == SQLITE_ROW) {
                const char* className = reinterpret_cast<const char*>(sqlite3_column_text(classStmt, 0));
                report.className = className ? std::string(className) : "";

                const char* trainer = reinterpret_cast<const char*>(sqlite3_column_text(classStmt, 1));
                report.trainer = trainer ? std::string(trainer) : "";
            }

            sqlite3_finalize(classStmt);
        }

        report.totalReservations = 0;
        report.fillRate = 0.0;
    }

    sqlite3_finalize(stmt);

    return report;
}

FinancialReport ReportService::getFinancialReport(const std::string& startDate, const std::string& endDate) {
    FinancialReport report;
    report.totalRevenue = 0.0;
    report.totalMemberships = 0;
    report.studentRevenue = 0.0;
    report.standardRevenue = 0.0;

    sqlite3* db = dbManager.getConnection();

    // Zapytanie pobieraj¹ce informacje finansowe w podanym okresie
    std::string query =
        "SELECT type, COUNT(*) as count, SUM(price) as revenue "
        "FROM memberships "
        "WHERE date(start_date) BETWEEN date(?) AND date(?) "
        "GROUP BY type";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, startDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, endDate.c_str(), -1, SQLITE_STATIC);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char* type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string membershipType = type ? std::string(type) : "";

        int count = sqlite3_column_int(stmt, 1);
        double revenue = sqlite3_column_double(stmt, 2);

        report.revenueByMembershipType[membershipType] = revenue;
        report.membershipCountByType[membershipType] = count;

        report.totalRevenue += revenue;
        report.totalMemberships += count;

        if (membershipType.find("student") != std::string::npos) {
            report.studentRevenue += revenue;
        }
        else {
            report.standardRevenue += revenue;
        }
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return report;
}