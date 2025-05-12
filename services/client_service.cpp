// services/client_service.cpp
#include "client_service.h"
#include <stdexcept>

ClientService::ClientService(ClientDAO& clientDao) : clientDao(clientDao) {
}

std::vector<Client> ClientService::getAllClients() {
    return clientDao.getAllClients();
}

std::unique_ptr<Client> ClientService::getClientById(int id) {
    return clientDao.getClientById(id);
}

int ClientService::addClient(const Client& client) {
    // Tutaj mo¿na dodaæ walidacjê danych klienta przed dodaniem do bazy
    if (client.getFirstName().empty() || client.getLastName().empty()) {
        throw std::invalid_argument("Imiê i nazwisko nie mog¹ byæ puste");
    }

    // Sprawdzenie poprawnoœci adresu email (prosta walidacja)
    if (!client.getEmail().empty() && client.getEmail().find('@') == std::string::npos) {
        throw std::invalid_argument("Nieprawid³owy format adresu email");
    }

    return clientDao.addClient(client);
}

bool ClientService::updateClient(const Client& client) {
    // Walidacja podobna jak przy dodawaniu
    if (client.getFirstName().empty() || client.getLastName().empty()) {
        throw std::invalid_argument("Imiê i nazwisko nie mog¹ byæ puste");
    }

    if (!client.getEmail().empty() && client.getEmail().find('@') == std::string::npos) {
        throw std::invalid_argument("Nieprawid³owy format adresu email");
    }

    return clientDao.updateClient(client);
}

bool ClientService::removeClient(int id) {
    // Przed usuniêciem klienta mo¿na sprawdziæ, czy nie ma aktywnych karnetów lub rezerwacji
    // Tutaj uproszczona wersja
    return clientDao.deleteClient(id);
}

std::vector<Client> ClientService::searchClients(const std::string& keyword) {
    if (keyword.empty()) {
        return getAllClients();
    }
    return clientDao.searchClients(keyword);
}