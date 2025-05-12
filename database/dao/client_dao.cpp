// database/dao/client_dao.cpp
#include "client_dao.h"
#include <sqlite3.h>
#include <iostream>

ClientDAO::ClientDAO(DBManager& dbManager) : dbManager(dbManager) {
}

std::vector<Client> ClientDAO::getAllClients() {
    std::vector<Client> clients;

    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, first_name, last_name, email, phone, birth_date, registration_date, notes FROM clients";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        clients.push_back(rowToClient(stmt));
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return clients;
}

std::unique_ptr<Client> ClientDAO::getClientById(int id) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, first_name, last_name, email, phone, birth_date, registration_date, notes FROM clients WHERE id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        auto client = std::make_unique<Client>(rowToClient(stmt));
        sqlite3_finalize(stmt);
        return client;
    }
    else {
        sqlite3_finalize(stmt);
        return nullptr;
    }
}

int ClientDAO::addClient(const Client& client) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "INSERT INTO clients (first_name, last_name, email, phone, birth_date, registration_date, notes) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, client.getFirstName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, client.getLastName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, client.getEmail().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, client.getPhone().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, client.getBirthDate().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, client.getRegistrationDate().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, client.getNotes().c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return dbManager.getLastInsertRowId();
}

bool ClientDAO::updateClient(const Client& client) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "UPDATE clients SET first_name = ?, last_name = ?, email = ?, "
        "phone = ?, birth_date = ?, registration_date = ?, notes = ? "
        "WHERE id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, client.getFirstName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, client.getLastName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, client.getEmail().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, client.getPhone().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, client.getBirthDate().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, client.getRegistrationDate().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, client.getNotes().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 8, client.getId());

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool ClientDAO::deleteClient(int id) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "DELETE FROM clients WHERE id = ?";

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

std::vector<Client> ClientDAO::searchClients(const std::string& keyword) {
    std::vector<Client> clients;

    sqlite3* db = dbManager.getConnection();
    std::string query = "SELECT id, first_name, last_name, email, phone, birth_date, registration_date, notes "
        "FROM clients WHERE first_name LIKE ? OR last_name LIKE ? OR email LIKE ? OR phone LIKE ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    std::string pattern = "%" + keyword + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, pattern.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, pattern.c_str(), -1, SQLITE_STATIC);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        clients.push_back(rowToClient(stmt));
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return clients;
}

Client ClientDAO::rowToClient(sqlite3_stmt* stmt) {
    int id = sqlite3_column_int(stmt, 0);

    const char* firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    const char* lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    const char* email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    const char* phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    const char* birthDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    const char* registrationDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    const char* notes = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));

    return Client(
        id,
        firstName ? std::string(firstName) : "",
        lastName ? std::string(lastName) : "",
        email ? std::string(email) : "",
        phone ? std::string(phone) : "",
        birthDate ? std::string(birthDate) : "",
        registrationDate ? std::string(registrationDate) : "",
        notes ? std::string(notes) : ""
    );
}