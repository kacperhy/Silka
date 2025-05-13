// utils/data_export.cpp
#include "data_export.h"
#include <fstream>
#include <sstream>
#include <algorithm>

DataExport::DataExport(ClientService& clientService, MembershipService& membershipService, ClassService& classService)
    : clientService(clientService), membershipService(membershipService), classService(classService) {
}

std::string DataExport::escapeCSV(const std::string& str) {
    // Jeśli string zawiera przecinek, cudzysłów lub znak nowej linii, zawijamy go w cudzysłowy
    if (str.find(',') != std::string::npos || str.find('"') != std::string::npos || 
        str.find('\n') != std::string::npos || str.find('\r') != std::string::npos) {
        
        std::string escaped = str;
        // Zamieniamy wszystkie cudzysłowy na podwójne cudzysłowy
        size_t pos = 0;
        while ((pos = escaped.find('"', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\"\"");
            pos += 2;
        }
        
        // Zawijamy całość w cudzysłowy
        return "\"" + escaped + "\"";
    }
    
    return str;
}

bool DataExport::exportClientsToCSV(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw ExportException("Nie można otworzyć pliku: " + filePath);
    }
    
    // Nagłówek CSV
    file << "id,first_name,last_name,email,phone,birth_date,registration_date,notes\n";
    
    // Dane
    auto clients = clientService.getAllClients();
    for (const auto& client : clients) {
        file << client.getId() << ","
             << escapeCSV(client.getFirstName()) << ","
             << escapeCSV(client.getLastName()) << ","
             << escapeCSV(client.getEmail()) << ","
             << escapeCSV(client.getPhone()) << ","
             << escapeCSV(client.getBirthDate()) << ","
             << escapeCSV(client.getRegistrationDate()) << ","
             << escapeCSV(client.getNotes()) << "\n";
    }
    
    file.close();
    return true;
}

bool DataExport::exportMembershipsToCSV(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw ExportException("Nie można otworzyć pliku: " + filePath);
    }
    
    // Nagłówek CSV
    file << "id,client_id,type,start_date,end_date,price,is_active\n";
    
    // Dane
    auto memberships = membershipService.getAllMemberships();
    for (const auto& membership : memberships) {
        file << membership.getId() << ","
             << membership.getClientId() << ","
             << escapeCSV(membership.getType()) << ","
             << escapeCSV(membership.getStartDate()) << ","
             << escapeCSV(membership.getEndDate()) << ","
             << membership.getPrice() << ","
             << (membership.getIsActive() ? "1" : "0") << "\n";
    }
    
    file.close();
    return true;
}

bool DataExport::exportClassesToCSV(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw ExportException("Nie można otworzyć pliku: " + filePath);
    }
    
    // Nagłówek CSV
    file << "id,name,trainer,max_participants,date,time,duration,description\n";
    
    // Dane
    auto classes = classService.getAllClasses();
    for (const auto& gymClass : classes) {
        file << gymClass.getId() << ","
             << escapeCSV(gymClass.getName()) << ","
             << escapeCSV(gymClass.getTrainer()) << ","
             << gymClass.getMaxParticipants() << ","
             << escapeCSV(gymClass.getDate()) << ","
             << escapeCSV(gymClass.getTime()) << ","
             << gymClass.getDuration() << ","
             << escapeCSV(gymClass.getDescription()) << "\n";
    }
    
    file.close();
    return true;
}

bool DataExport::exportReservationsToCSV(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw ExportException("Nie można otworzyć pliku: " + filePath);
    }
    
    // Nagłówek CSV
    file << "id,client_id,class_id,reservation_date,status\n";
    
    // Dane
    auto reservations = classService.getAllReservations();
    for (const auto& reservation : reservations) {
        file << reservation.getId() << ","
             << reservation.getClientId() << ","
             << reservation.getClassId() << ","
             << escapeCSV(reservation.getReservationDate()) << ","
             << escapeCSV(reservation.getStatus()) << "\n";
    }
    
    file.close();
    return true;
}

std::string DataExport::escapeJSON(const std::string& str) {
    std::string escaped;
    escaped.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\b': escaped += "\\b"; break;
            case '\f': escaped += "\\f"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    escaped += buf;
                } else {
                    escaped += c;
                }
        }
    }
    
    return escaped;
}

std::string DataExport::clientToJSON(const Client& client) {
    std::stringstream ss;
    
    ss << "{";
    ss << "\"id\":" << client.getId() << ",";
    ss << "\"firstName\":\"" << escapeJSON(client.getFirstName()) << "\",";
    ss << "\"lastName\":\"" << escapeJSON(client.getLastName()) << "\",";
    ss << "\"email\":\"" << escapeJSON(client.getEmail()) << "\",";
    ss << "\"phone\":\"" << escapeJSON(client.getPhone()) << "\",";
    ss << "\"birthDate\":\"" << escapeJSON(client.getBirthDate()) << "\",";
    ss << "\"registrationDate\":\"" << escapeJSON(client.getRegistrationDate()) << "\",";
    ss << "\"notes\":\"" << escapeJSON(client.getNotes()) << "\"";
    ss << "}";
    
    return ss.str();
}

std::string DataExport::membershipToJSON(const Membership& membership) {
    std::stringstream ss;
    
    ss << "{";
    ss << "\"id\":" << membership.getId() << ",";
    ss << "\"clientId\":" << membership.getClientId() << ",";
    ss << "\"type\":\"" << escapeJSON(membership.getType()) << "\",";
    ss << "\"startDate\":\"" << escapeJSON(membership.getStartDate()) << "\",";
    ss << "\"endDate\":\"" << escapeJSON(membership.getEndDate()) << "\",";
    ss << "\"price\":" << membership.getPrice() << ",";
    ss << "\"isActive\":" << (membership.getIsActive() ? "true" : "false");
    ss << "}";
    
    return ss.str();
}

std::string DataExport::gymClassToJSON(const GymClass& gymClass) {
    std::stringstream ss;
    
    ss << "{";
    ss << "\"id\":" << gymClass.getId() << ",";
    ss << "\"name\":\"" << escapeJSON(gymClass.getName()) << "\",";
    ss << "\"trainer\":\"" << escapeJSON(gymClass.getTrainer()) << "\",";
    ss << "\"maxParticipants\":" << gymClass.getMaxParticipants() << ",";
    ss << "\"date\":\"" << escapeJSON(gymClass.getDate()) << "\",";
    ss << "\"time\":\"" << escapeJSON(gymClass.getTime()) << "\",";
    ss << "\"duration\":" << gymClass.getDuration() << ",";
    ss << "\"description\":\"" << escapeJSON(gymClass.getDescription()) << "\"";
    ss << "}";
    
    return ss.str();
}

std::string DataExport::reservationToJSON(const Reservation& reservation) {
    std::stringstream ss;
    
    ss << "{";
    ss << "\"id\":" << reservation.getId() << ",";
    ss << "\"clientId\":" << reservation.getClientId() << ",";
    ss << "\"classId\":" << reservation.getClassId() << ",";
    ss << "\"reservationDate\":\"" << escapeJSON(reservation.getReservationDate()) << "\",";
    ss << "\"status\":\"" << escapeJSON(reservation.getStatus()) << "\"";
    ss << "}";
    
    return ss.str();
}

bool DataExport::exportClientsToJSON(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw ExportException("Nie można otworzyć pliku: " + filePath);
    }
    
    auto clients = clientService.getAllClients();
    
    file << "{\n";
    file << "  \"clients\": [\n";
    
    for (size_t i = 0; i < clients.size(); ++i) {
        file << "    " << clientToJSON(clients[i]);
        if (i < clients.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    return true;
}

bool DataExport::exportMembershipsToJSON(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw ExportException("Nie można otworzyć pliku: " + filePath);
    }
    
    auto memberships = membershipService.getAllMemberships();
    
    file << "{\n";
    file << "  \"memberships\": [\n";
    
    for (size_t i = 0; i < memberships.size(); ++i) {
        file << "    " << membershipToJSON(memberships[i]);
        if (i < memberships.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    return true;
}

// utils/data_export.cpp (kontynuacja)
bool DataExport::exportClassesToJSON(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw ExportException("Nie można otworzyć pliku: " + filePath);
    }
    
    auto classes = classService.getAllClasses();
    
    file << "{\n";
    file << "  \"classes\": [\n";
    
    for (size_t i = 0; i < classes.size(); ++i) {
        file << "    " << gymClassToJSON(classes[i]);
        if (i < classes.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    return true;
}

bool DataExport::exportReservationsToJSON(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw ExportException("Nie można otworzyć pliku: " + filePath);
    }
    
    auto reservations = classService.getAllReservations();
    
    file << "{\n";
    file << "  \"reservations\": [\n";
    
    for (size_t i = 0; i < reservations.size(); ++i) {
        file << "    " << reservationToJSON(reservations[i]);
        if (i < reservations.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    return true;
}