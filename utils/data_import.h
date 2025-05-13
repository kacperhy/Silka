// utils/data_import.h
#ifndef DATA_IMPORT_H
#define DATA_IMPORT_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "../models/client.h"
#include "../models/membership.h"
#include "../models/gym_class.h"
#include "../services/client_service.h"
#include "../services/membership_service.h"
#include "../services/class_service.h"

class ImportException : public std::runtime_error {
public:
    explicit ImportException(const std::string& message) : std::runtime_error(message) {}
};

class DataImport {
public:
    DataImport(ClientService& clientService, MembershipService& membershipService, ClassService& classService);
    
    // Metody importu plików CSV
    std::vector<Client> importClientsFromCSV(const std::string& filePath);
    std::vector<Membership> importMembershipsFromCSV(const std::string& filePath);
    std::vector<GymClass> importClassesFromCSV(const std::string& filePath);
    
    // Metody importu plików JSON
    std::vector<Client> importClientsFromJSON(const std::string& filePath);
    std::vector<Membership> importMembershipsFromJSON(const std::string& filePath);
    std::vector<GymClass> importClassesFromJSON(const std::string& filePath);
    
    // Metody do zapisywania zaimportowanych danych do bazy
    void saveImportedClients(const std::vector<Client>& clients);
    void saveImportedMemberships(const std::vector<Membership>& memberships);
    void saveImportedClasses(const std::vector<GymClass>& classes);
    
private:
    ClientService& clientService;
    MembershipService& membershipService;
    ClassService& classService;
    
    // Metody pomocnicze do parsowania CSV
    std::vector<std::vector<std::string>> parseCSV(const std::string& filePath, char delimiter = ',');
    Client parseClientRow(const std::vector<std::string>& row);
    Membership parseMembershipRow(const std::vector<std::string>& row);
    GymClass parseClassRow(const std::vector<std::string>& row);
    
    // Metody pomocnicze do parsowania JSON
    std::string readJSONFile(const std::string& filePath);
};

#endif // DATA_IMPORT_H