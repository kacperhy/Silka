// utils/data_export.h
#ifndef DATA_EXPORT_H
#define DATA_EXPORT_H

#include <string>
#include <vector>
#include <stdexcept>
#include "../models/client.h"
#include "../models/membership.h"
#include "../models/gym_class.h"
#include "../models/reservation.h"
#include "../services/client_service.h"
#include "../services/membership_service.h"
#include "../services/class_service.h"

class ExportException : public std::runtime_error {
public:
    explicit ExportException(const std::string& message) : std::runtime_error(message) {}
};

class DataExport {
public:
    DataExport(ClientService& clientService, MembershipService& membershipService, ClassService& classService);
    
    // Metody eksportu do CSV
    bool exportClientsToCSV(const std::string& filePath);
    bool exportMembershipsToCSV(const std::string& filePath);
    bool exportClassesToCSV(const std::string& filePath);
    bool exportReservationsToCSV(const std::string& filePath);
    
    // Metody eksportu do JSON
    bool exportClientsToJSON(const std::string& filePath);
    bool exportMembershipsToJSON(const std::string& filePath);
    bool exportClassesToJSON(const std::string& filePath);
    bool exportReservationsToJSON(const std::string& filePath);
    
private:
    ClientService& clientService;
    MembershipService& membershipService;
    ClassService& classService;
    
    // Metody pomocnicze do formatowania CSV
    std::string escapeCSV(const std::string& str);
    
    // Metody pomocnicze do formatowania JSON
    std::string clientToJSON(const Client& client);
    std::string membershipToJSON(const Membership& membership);
    std::string gymClassToJSON(const GymClass& gymClass);
    std::string reservationToJSON(const Reservation& reservation);
    std::string escapeJSON(const std::string& str);
};

#endif // DATA_EXPORT_H