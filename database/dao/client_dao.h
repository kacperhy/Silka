// database/dao/client_dao.h
#ifndef CLIENT_DAO_H
#define CLIENT_DAO_H

#include <vector>
#include <memory>
#include <string>
#include "../../models/client.h"
#include "../db_manager.h"

class ClientDAO {
public:
    explicit ClientDAO(DBManager& dbManager);

    // Metody dostêpu do danych
    std::vector<Client> getAllClients();
    std::unique_ptr<Client> getClientById(int id);
    int addClient(const Client& client);
    bool updateClient(const Client& client);
    bool deleteClient(int id);
    std::vector<Client> searchClients(const std::string& keyword);

private:
    DBManager& dbManager;

    // Metoda pomocnicza do konwersji wiersza rezultatu zapytania na obiekt klienta
    Client rowToClient(sqlite3_stmt* stmt);
};

#endif // CLIENT_DAO_H