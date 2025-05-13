// database/db_manager.cpp
#include "db_manager.h"
#include <iostream>

DBManager::DBManager(const std::string& dbPath) : dbPath(dbPath), connection(nullptr), isOpen(false) {
}

DBManager::~DBManager() {
    if (isOpen) {
        close();
    }
}

void DBManager::open() {
    if (isOpen) {
        return;
    }

    int rc = sqlite3_open(dbPath.c_str(), &connection);
    if (rc != SQLITE_OK) {
        std::string errorMsg = "Nie mo¿na otworzyæ bazy danych: ";
        errorMsg += sqlite3_errmsg(connection);
        sqlite3_close(connection);
        throw DatabaseException(errorMsg);
    }

    isOpen = true;

    // W³¹czenie obs³ugi kluczy obcych i kodowania UTF-8
    executeQuery("PRAGMA foreign_keys = ON");
    executeQuery("PRAGMA encoding = 'UTF-8'");

    // Tworzenie tabel, jeœli nie istniej¹ - ZMIANA W DEFINICJI TABELI CLIENTS
    executeQuery("CREATE TABLE IF NOT EXISTS clients ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT," // DODANE S£OWO KLUCZOWE AUTOINCREMENT
        "first_name TEXT NOT NULL,"
        "last_name TEXT NOT NULL,"
        "email TEXT,"
        "phone TEXT,"
        "birth_date TEXT,"
        "registration_date TEXT NOT NULL,"
        "notes TEXT"
        ")");

    // Pozosta³e tabele pozostaj¹ bez zmian
    executeQuery("CREATE TABLE IF NOT EXISTS memberships ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "client_id INTEGER NOT NULL,"
        "type TEXT NOT NULL,"
        "start_date TEXT NOT NULL,"
        "end_date TEXT NOT NULL,"
        "price REAL NOT NULL,"
        "is_active INTEGER NOT NULL,"
        "FOREIGN KEY (client_id) REFERENCES clients (id)"
        ")");

    executeQuery("CREATE TABLE IF NOT EXISTS classes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "trainer TEXT NOT NULL,"
        "max_participants INTEGER NOT NULL,"
        "date TEXT NOT NULL,"
        "time TEXT NOT NULL,"
        "duration INTEGER NOT NULL,"
        "description TEXT"
        ")");

    executeQuery("CREATE TABLE IF NOT EXISTS reservations ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "client_id INTEGER NOT NULL,"
        "class_id INTEGER NOT NULL,"
        "reservation_date TEXT NOT NULL,"
        "status TEXT NOT NULL,"
        "FOREIGN KEY (client_id) REFERENCES clients (id),"
        "FOREIGN KEY (class_id) REFERENCES classes (id)"
        ")");
}

void DBManager::close() {
    if (!isOpen) {
        return;
    }

    sqlite3_close(connection);
    connection = nullptr;
    isOpen = false;
}

void DBManager::beginTransaction() {
    executeQuery("BEGIN TRANSACTION");
}

void DBManager::commit() {
    executeQuery("COMMIT");
}

void DBManager::rollback() {
    executeQuery("ROLLBACK");
}

void DBManager::executeQuery(const std::string& query) {
    if (!isOpen) {
        throw DatabaseException("Baza danych nie jest otwarta");
    }

    char* errorMsg = nullptr;
    int rc = sqlite3_exec(connection, query.c_str(), nullptr, nullptr, &errorMsg);

    if (rc != SQLITE_OK) {
        std::string error = errorMsg ? errorMsg : "Nieznany b³¹d";
        sqlite3_free(errorMsg);
        throw DatabaseException("B³¹d wykonania zapytania: " + error);
    }
}

int DBManager::getLastInsertRowId() {
    return sqlite3_last_insert_rowid(connection);
}

sqlite3* DBManager::getConnection() const {
    return connection;
}