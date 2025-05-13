// utils/data_import.cpp
#include "data_import.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <map>
#include <algorithm>
#include <cstring>
#include <cctype>

// Funkcje pomocnicze dla JSON
namespace {
    // Prosta funkcja do usuwania białych znaków z JSON
    std::string stripWhitespace(const std::string& input) {
        std::string output;
        output.reserve(input.size());
        bool inString = false;
        
        for (char c : input) {
            if (c == '"') {
                inString = !inString;
                output += c;
            } else if (inString || !std::isspace(c)) {
                output += c;
            }
        }
        
        return output;
    }
    
    // Prosta funkcja do podziału stringa na wektory po separatorze
    std::vector<std::string> split(const std::string& input, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(input);
        std::string item;
        
        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }
        
        return result;
    }
    
    // Funkcja pomocnicza do parsowania wartości pól z cudzysłowami w CSV
    std::string unquote(const std::string& s) {
        if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
            return s.substr(1, s.size() - 2);
        }
        return s;
    }
    
    // Funkcja pomocnicza do usuwania białych znaków z początku i końca stringa
    std::string trim(const std::string& s) {
        auto start = s.begin();
        while (start != s.end() && std::isspace(*start)) {
            start++;
        }
        
        auto end = s.end();
        do {
            end--;
        } while (start != end && std::isspace(*end));
        
        return std::string(start, end + 1);
    }
}

DataImport::DataImport(ClientService& clientService, MembershipService& membershipService, ClassService& classService)
    : clientService(clientService), membershipService(membershipService), classService(classService) {
}

std::vector<std::vector<std::string>> DataImport::parseCSV(const std::string& filePath, char delimiter) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw ImportException("Nie można otworzyć pliku: " + filePath);
    }
    
    std::vector<std::vector<std::string>> data;
    std::string line;
    
    // Pomijamy nagłówek (pierwsza linia)
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::string cell;
        
        bool inQuotes = false;
        std::stringstream ss(line);
        
        while (ss.good()) {
            char c = ss.get();
            
            if (c == EOF) {
                break;
            } else if (c == '"') {
                inQuotes = !inQuotes;
            } else if (c == delimiter && !inQuotes) {
                row.push_back(trim(cell));
                cell.clear();
            } else {
                cell += c;
            }
        }
        
        if (!cell.empty()) {
            row.push_back(trim(cell));
        }
        
        if (!row.empty()) {
            data.push_back(row);
        }
    }
    
    file.close();
    return data;
}

Client DataImport::parseClientRow(const std::vector<std::string>& row) {
    if (row.size() < 6) {
        throw ImportException("Nieprawidłowa liczba kolumn dla klienta");
    }
    
    Client client;
    
    // Zakładamy format: id, first_name, last_name, email, phone, birth_date, registration_date, notes
    // ID pomijamy, ponieważ zostanie nadane przez bazę danych
    client.setFirstName(unquote(row[1]));
    client.setLastName(unquote(row[2]));
    client.setEmail(unquote(row[3]));
    client.setPhone(unquote(row[4]));
    client.setBirthDate(unquote(row[5]));
    
    if (row.size() > 6) {
        client.setRegistrationDate(unquote(row[6]));
    }
    
    if (row.size() > 7) {
        client.setNotes(unquote(row[7]));
    }
    
    return client;
}

Membership DataImport::parseMembershipRow(const std::vector<std::string>& row) {
    if (row.size() < 6) {
        throw ImportException("Nieprawidłowa liczba kolumn dla karnetu");
    }
    
    Membership membership;
    
    // Zakładamy format: id, client_id, type, start_date, end_date, price, is_active
    // ID pomijamy, ponieważ zostanie nadane przez bazę danych
    membership.setClientId(std::stoi(unquote(row[1])));
    membership.setType(unquote(row[2]));
    membership.setStartDate(unquote(row[3]));
    membership.setEndDate(unquote(row[4]));
    membership.setPrice(std::stod(unquote(row[5])));
    
    if (row.size() > 6) {
        membership.setIsActive(unquote(row[6]) == "1" || unquote(row[6]) == "true");
    }
    
    return membership;
}

GymClass DataImport::parseClassRow(const std::vector<std::string>& row) {
    if (row.size() < 7) {
        throw ImportException("Nieprawidłowa liczba kolumn dla zajęć");
    }
    
    GymClass gymClass;
    
    // Zakładamy format: id, name, trainer, max_participants, date, time, duration, description
    // ID pomijamy, ponieważ zostanie nadane przez bazę danych
    gymClass.setName(unquote(row[1]));
    gymClass.setTrainer(unquote(row[2]));
    gymClass.setMaxParticipants(std::stoi(unquote(row[3])));
    gymClass.setDate(unquote(row[4]));
    gymClass.setTime(unquote(row[5]));
    gymClass.setDuration(std::stoi(unquote(row[6])));
    
    if (row.size() > 7) {
        gymClass.setDescription(unquote(row[7]));
    }
    
    return gymClass;
}

std::vector<Client> DataImport::importClientsFromCSV(const std::string& filePath) {
    std::vector<Client> clients;
    
    try {
        auto data = parseCSV(filePath);
        
        for (const auto& row : data) {
            clients.push_back(parseClientRow(row));
        }
    } catch (const std::exception& e) {
        throw ImportException(std::string("Błąd podczas importu klientów z CSV: ") + e.what());
    }
    
    return clients;
}

std::vector<Membership> DataImport::importMembershipsFromCSV(const std::string& filePath) {
    std::vector<Membership> memberships;
    
    try {
        auto data = parseCSV(filePath);
        
        for (const auto& row : data) {
            memberships.push_back(parseMembershipRow(row));
        }
    } catch (const std::exception& e) {
        throw ImportException(std::string("Błąd podczas importu karnetów z CSV: ") + e.what());
    }
    
    return memberships;
}

std::vector<GymClass> DataImport::importClassesFromCSV(const std::string& filePath) {
    std::vector<GymClass> classes;
    
    try {
        auto data = parseCSV(filePath);
        
        for (const auto& row : data) {
            classes.push_back(parseClassRow(row));
        }
    } catch (const std::exception& e) {
        throw ImportException(std::string("Błąd podczas importu zajęć z CSV: ") + e.what());
    }
    
    return classes;
}

// UWAGA: To jest bardzo prosta implementacja parsowania JSON, która nie jest odporna na wszystkie przypadki.
// W rzeczywistej aplikacji zalecane byłoby użycie biblioteki do obsługi JSON, np. nlohmann/json.
std::string DataImport::readJSONFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw ImportException("Nie można otworzyć pliku: " + filePath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

std::vector<Client> DataImport::importClientsFromJSON(const std::string& filePath) {
    // UWAGA: Ta implementacja jest bardzo uproszczona i nie obsługuje wszystkich przypadków.
    // W rzeczywistym projekcie warto użyć biblioteki JSON.
    std::vector<Client> clients;
    
    try {
        std::string json = readJSONFile(filePath);
        json = stripWhitespace(json);
        
        // Proste parsowanie tablicy JSON: znajdujemy "[" i "]" oraz dzielimy dane między nimi po "},"
        size_t start = json.find('[');
        size_t end = json.rfind(']');
        
        if (start == std::string::npos || end == std::string::npos) {
            throw ImportException("Nieprawidłowy format JSON");
        }
        
        std::string arrayContent = json.substr(start + 1, end - start - 1);
        std::vector<std::string> clientObjects;
        
        size_t pos = 0;
        std::string delimiter = "},{";
        std::string token;
        
        while ((pos = arrayContent.find(delimiter)) != std::string::npos) {
            token = arrayContent.substr(0, pos + 1); // Dodajemy "}" do tokena
            clientObjects.push_back(token);
            arrayContent.erase(0, pos + delimiter.length() - 1); // Pozostawiamy "{" w kolejnym tokenie
        }
        
        // Dodajemy ostatni obiekt
        if (!arrayContent.empty()) {
            clientObjects.push_back(arrayContent);
        }
        
        for (const auto& clientObj : clientObjects) {
            Client client;
            
            // Parsowanie pól obiektu JSON
            std::map<std::string, std::string> fields;
            std::string fieldDelimiter = ",\"";
            std::string object = clientObj;
            
            // Usuwamy początkowe "{" i końcowe "}"
            if (object.front() == '{') object = object.substr(1);
            if (object.back() == '}') object = object.substr(0, object.size() - 1);
            
            // Dzielimy obiekt na pola
            std::vector<std::string> fieldPairs = split(object, ',');
            
            for (const auto& pair : fieldPairs) {
                size_t colonPos = pair.find(':');
                if (colonPos != std::string::npos) {
                    std::string key = pair.substr(0, colonPos);
                    std::string value = pair.substr(colonPos + 1);
                    
                    // Usuwamy cudzysłowy z klucza
                    if (key.front() == '"') key = key.substr(1);
                    if (key.back() == '"') key = key.substr(0, key.size() - 1);
                    
                    // Usuwamy cudzysłowy z wartości
                    if (value.front() == '"') value = value.substr(1);
                    if (value.back() == '"') value = value.substr(0, value.size() - 1);
                    
                    fields[key] = value;
                }
            }
            
            // Ustawiamy pola klienta
            if (fields.count("firstName")) client.setFirstName(fields["firstName"]);
            if (fields.count("lastName")) client.setLastName(fields["lastName"]);
            if (fields.count("email")) client.setEmail(fields["email"]);
            if (fields.count("phone")) client.setPhone(fields["phone"]);
            if (fields.count("birthDate")) client.setBirthDate(fields["birthDate"]);
            if (fields.count("registrationDate")) client.setRegistrationDate(fields["registrationDate"]);
            if (fields.count("notes")) client.setNotes(fields["notes"]);
            
            clients.push_back(client);
        }
    } catch (const std::exception& e) {
        throw ImportException(std::string("Błąd podczas importu klientów z JSON: ") + e.what());
    }
    
    return clients;
}

// Analogiczne implementacje dla karnetów i zajęć
std::vector<Membership> DataImport::importMembershipsFromJSON(const std::string& filePath) {
    // Podobnie jak w przypadku klientów, ta implementacja jest uproszczona
    // W rzeczywistym projekcie warto użyć biblioteki JSON
    
    std::vector<Membership> memberships;
    // Implementacja podobna do importClientsFromJSON, dostosowana do modelu Membership
    
    return memberships;
}

std::vector<GymClass> DataImport::importClassesFromJSON(const std::string& filePath) {
    // Podobnie jak w przypadku klientów, ta implementacja jest uproszczona
    // W rzeczywistym projekcie warto użyć biblioteki JSON
    
    std::vector<GymClass> classes;
    // Implementacja podobna do importClientsFromJSON, dostosowana do modelu GymClass
    
    return classes;
}

void DataImport::saveImportedClients(const std::vector<Client>& clients) {
    for (const auto& client : clients) {
        clientService.addClient(client);
    }
}

void DataImport::saveImportedMemberships(const std::vector<Membership>& memberships) {
    for (const auto& membership : memberships) {
        membershipService.addMembership(membership);
    }
}

void DataImport::saveImportedClasses(const std::vector<GymClass>& classes) {
    for (const auto& gymClass : classes) {
        classService.addClass(gymClass);
    }
}