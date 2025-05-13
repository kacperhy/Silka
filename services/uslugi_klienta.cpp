// services/client_service.cpp
#include "client_service.h"
#include <stdexcept>
#include <iostream>
ClientService::ClientService(ClientDAO& clientDao) : clientDao(clientDao) {
}

std::vector<Client> ClientService::getAllClients() {
    return clientDao.getAllClients();
}

std::unique_ptr<Client> ClientService::getClientById(int id) {
    return clientDao.getClientById(id);
}

int ClientService::addClient(const Client& client) {
    // Podstawowa walidacja
    if (client.getFirstName().empty()) {
        throw std::invalid_argument("Imiê nie mo¿e byæ puste");
    }

    if (client.getLastName().empty()) {
        throw std::invalid_argument("Nazwisko nie mo¿e byæ puste");
    }

    // SprawdŸ, czy dane s¹ niepuste przed dodaniem
    std::cout << "ClientService - Dane do dodania:\n";
    std::cout << "Imiê: [" << client.getFirstName() << "]\n";
    std::cout << "Nazwisko: [" << client.getLastName() << "]\n";
    std::cout << "Email: [" << client.getEmail() << "]\n";

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