// database/db_manager.h
#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <string>
#include <memory>
#include <sqlite3.h>
#include <stdexcept>

class DatabaseException : public std::runtime_error {
public:
    explicit DatabaseException(const std::string& message) : std::runtime_error(message) {}
};

class DBManager {
public:
    DBManager(const std::string& dbPath);
    ~DBManager();

    // Metody do zarz�dzania baz� danych
    void open();
    void close();
    void beginTransaction();
    void commit();
    void rollback();

    // Metody do wykonywania zapyta�
    void executeQuery(const std::string& query);
    int getLastInsertRowId();

    // Dost�p do po��czenia z baz�
    sqlite3* getConnection() const;

private:
    std::string dbPath;
    sqlite3* connection;
    bool isOpen;
};

#endif // DB_MANAGER_H