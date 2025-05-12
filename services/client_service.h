// services/client_service.h
#ifndef CLIENT_SERVICE_H
#define CLIENT_SERVICE_H

#include <vector>
#include <memory>
#include <string>
#include "../models/client.h"
#include "../database/dao/client_dao.h"

class ClientService {
public:
    explicit ClientService(ClientDAO& clientDao);
    
    // Metody biznesowe
    std::vector<Client> getAllClients();
    std::unique_ptr<Client> getClientById(int id);
    int addClient(const Client& client);
    bool updateClient(const Client& client);
    bool removeClient(int id);
    std::vector<Client> searchClients(const std::string& keyword);
    
private:
    ClientDAO& clientDao;
};

#endif // CLIENT_SERVICE_H