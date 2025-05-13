// database/dao/membership_dao.cpp
#include "membership_dao.h"
#include <sqlite3.h>
#include <iostream>

MembershipDAO::MembershipDAO(DBManager& dbManager) : dbManager(dbManager) {
}

std::vector<Membership> MembershipDAO::getAllMemberships() {
    std::vector<Membership> memberships;

    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, client_id, type, start_date, end_date, price, is_active FROM memberships";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        memberships.push_back(rowToMembership(stmt));
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return memberships;
}

std::vector<Membership> MembershipDAO::getMembershipsByClientId(int clientId) {
    std::vector<Membership> memberships;

    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, client_id, type, start_date, end_date, price, is_active FROM memberships "
        "WHERE client_id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, clientId);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        memberships.push_back(rowToMembership(stmt));
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return memberships;
}

std::unique_ptr<Membership> MembershipDAO::getMembershipById(int id) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, client_id, type, start_date, end_date, price, is_active FROM memberships "
        "WHERE id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        auto membership = std::make_unique<Membership>(rowToMembership(stmt));
        sqlite3_finalize(stmt);
        return membership;
    }
    else {
        sqlite3_finalize(stmt);
        return nullptr;
    }
}

std::unique_ptr<Membership> MembershipDAO::getActiveMembershipForClient(int clientId) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, client_id, type, start_date, end_date, price, is_active FROM memberships "
        "WHERE client_id = ? AND is_active = 1 AND date(end_date) >= date('now') "
        "ORDER BY end_date DESC LIMIT 1";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, clientId);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        auto membership = std::make_unique<Membership>(rowToMembership(stmt));
        sqlite3_finalize(stmt);
        return membership;
    }
    else {
        sqlite3_finalize(stmt);
        return nullptr;
    }
}

int MembershipDAO::addMembership(const Membership& membership) {
    // Dodaj debug
    std::cout << "\nDEBUG MembershipDAO::addMembership - Zapisywane dane:\n";
    std::cout << "  ClientID: " << membership.getClientId() << "\n";
    std::cout << "  Type: [" << membership.getType() << "]\n";
    std::cout << "  StartDate: [" << membership.getStartDate() << "]\n";
    std::cout << "  EndDate: [" << membership.getEndDate() << "]\n";
    std::cout << "  Price: [" << membership.getPrice() << "]\n";
    std::cout << "  IsActive: [" << (membership.getIsActive() ? "Tak" : "Nie") << "]\n";

    try {
        // ZnajdŸ najwy¿sze ID
        sqlite3* db = dbManager.getConnection();
        const char* maxIdQuery = "SELECT MAX(id) FROM memberships";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, maxIdQuery, -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
        }

        int nextId = 1; // Domyœlne ID, jeœli tabela jest pusta

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int maxId = sqlite3_column_int(stmt, 0);
            nextId = maxId + 1;
        }

        sqlite3_finalize(stmt);

        // Teraz dodajemy karnet z ustalonym ID
        const char* insertQuery = "INSERT INTO memberships (id, client_id, type, start_date, end_date, price, is_active) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)";

        rc = sqlite3_prepare_v2(db, insertQuery, -1, &stmt, nullptr);

        if (rc != SQLITE_OK) {
            throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
        }

        sqlite3_bind_int(stmt, 1, nextId);
        sqlite3_bind_int(stmt, 2, membership.getClientId());
        sqlite3_bind_text(stmt, 3, membership.getType().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, membership.getStartDate().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, membership.getEndDate().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 6, membership.getPrice());
        sqlite3_bind_int(stmt, 7, membership.getIsActive() ? 1 : 0);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE) {
            throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
        }

        return nextId;
    }
    catch (const std::exception& e) {
        throw DatabaseException(std::string("B³¹d podczas dodawania karnetu: ") + e.what());
    }
    catch (...) {
        throw DatabaseException("Nieznany b³¹d podczas dodawania karnetu");
    }
}

bool MembershipDAO::updateMembership(const Membership& membership) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "UPDATE memberships SET client_id = ?, type = ?, start_date = ?, "
        "end_date = ?, price = ?, is_active = ? WHERE id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, membership.getClientId());
    sqlite3_bind_text(stmt, 2, membership.getType().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, membership.getStartDate().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, membership.getEndDate().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 5, membership.getPrice());
    sqlite3_bind_int(stmt, 6, membership.getIsActive() ? 1 : 0);
    sqlite3_bind_int(stmt, 7, membership.getId());

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool MembershipDAO::deleteMembership(int id) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "DELETE FROM memberships WHERE id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

Membership MembershipDAO::rowToMembership(sqlite3_stmt* stmt) {
    int id = sqlite3_column_int(stmt, 0);
    int clientId = sqlite3_column_int(stmt, 1);

    const char* type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    const char* startDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    const char* endDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

    double price = sqlite3_column_double(stmt, 5);
    bool isActive = sqlite3_column_int(stmt, 6) != 0;

    return Membership(
        id,
        clientId,
        type ? std::string(type) : "",
        startDate ? std::string(startDate) : "",
        endDate ? std::string(endDate) : "",
        price,
        isActive
    );
}