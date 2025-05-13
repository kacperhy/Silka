// tests/main.cpp
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <windows.h>


#include "../database/db_manager.h"
#include "../database/dao/client_dao.h"
#include "../database/dao/membership_dao.h"
#include "../database/dao/class_dao.h"
#include "../services/client_service.h"
#include "../services/membership_service.h"
#include "../services/class_service.h"
#include "../services/report_service.h"
#include "../utils/data_import.h"
#include "../utils/data_export.h"
#include "../models/client.h"
#include "../models/membership.h"
#include "../models/gym_class.h"
#include "../models/reservation.h"

void removeClientById(ClientService& clientService) {
    int clientId;

    std::cout << "Podaj ID klienta do usunięcia: ";
    std::cin >> clientId;

    auto client = clientService.getClientById(clientId);
    if (!client) {
        std::cout << "Nie znaleziono klienta o ID: " << clientId << std::endl;
        return;
    }

    std::cout << "Czy na pewno chcesz usunąć klienta " << client->getFullName() << " (ID: " << clientId << ")? (T/N): ";
    char confirm;
    std::cin >> confirm;

    if (confirm == 'T' || confirm == 't') {
        bool success = clientService.removeClient(clientId);

        if (success) {
            std::cout << "Klient został usunięty." << std::endl;
        }
        else {
            std::cout << "Wystąpił błąd podczas usuwania klienta." << std::endl;
        }
    }
    else {
        std::cout << "Anulowano usuwanie klienta." << std::endl;
    }
}

// Funkcja wyświetlająca menu główne
void displayMenu() {
    std::cout << "\n===== System zarządzania siłownią =====\n";
    std::cout << "1. Dodaj nowego klienta\n";
    std::cout << "2. Wyświetl wszystkich klientów\n";
    std::cout << "3. Wyszukaj klienta\n";
    std::cout << "4. Dodaj karnet dla klienta\n";
    std::cout << "5. Wyświetl karnety klienta\n";
    std::cout << "6. Dodaj zajęcia grupowe\n";
    std::cout << "7. Wyświetl wszystkie zajęcia\n";
    std::cout << "8. Zarezerwuj miejsce na zajęciach\n";
    std::cout << "9. Wygeneruj raport aktywności\n";
    std::cout << "10. Usuń klienta\n"; 
    std::cout << "\n=== Import/Export danych ===\n";
    std::cout << "11. Import danych z pliku CSV\n";
    std::cout << "12. Import danych z pliku JSON\n";
    std::cout << "13. Eksport danych do pliku CSV\n";
    std::cout << "14. Eksport danych do pliku JSON\n";
    std::cout << "0. Wyjście\n";
    std::cout << "Wybierz opcję: ";
}

// Funkcja do dodawania nowego klienta
void addNewClient(ClientService& clientService) {
    std::string firstName, lastName, email, phone, birthDate, notes;

    std::cin.ignore();
    std::cout << "Podaj imię: ";
    std::getline(std::cin, firstName);

    std::cout << "Podaj nazwisko: ";
    std::getline(std::cin, lastName);

    std::cout << "Podaj email: ";
    std::getline(std::cin, email);

    std::cout << "Podaj telefon: ";
    std::getline(std::cin, phone);

    std::cout << "Podaj datę urodzenia (YYYY-MM-DD): ";
    std::getline(std::cin, birthDate);

    std::cout << "Podaj uwagi: ";
    std::getline(std::cin, notes);

    Client client(-1, firstName, lastName, email, phone, birthDate, "", notes);

    try {
        int id = clientService.addClient(client);
        std::cout << "Dodano nowego klienta z ID: " << id << std::endl;
    }
    catch (const DatabaseException& e) {
        std::cerr << "Błąd bazy danych: " << e.what() << std::endl;
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Błąd walidacji danych: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Wystąpił błąd: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Wystąpił nieznany błąd" << std::endl;
    }
}

// Funkcja do wyświetlania wszystkich klientów
void displayAllClients(ClientService& clientService) {
    auto clients = clientService.getAllClients();

    if (clients.empty()) {
        std::cout << "Brak klientów w bazie danych." << std::endl;
        return;
    }

    std::cout << "\n=== Lista klientów ===\n";
    std::cout << std::left
        << std::setw(5) << "ID"
        << std::setw(30) << "Imię i nazwisko"
        << std::setw(30) << "Email"
        << std::setw(15) << "Telefon"
        << std::setw(15) << "Data rejestracji"
        << std::endl;
    std::cout << std::string(95, '-') << std::endl;

    for (const auto& client : clients) {
        std::cout << std::left
            << std::setw(5) << client.getId()
            << std::setw(30) << client.getFirstName() + " " + client.getLastName()
            << std::setw(30) << client.getEmail()
            << std::setw(15) << client.getPhone()
            << std::setw(15) << client.getRegistrationDate()
            << std::endl;
    }
}

// Funkcja do wyszukiwania klientów
void searchClients(ClientService& clientService) {
    std::string keyword;
    std::cin.ignore();
    std::cout << "Podaj frazę do wyszukania: ";
    std::getline(std::cin, keyword);

    auto clients = clientService.searchClients(keyword);

    if (clients.empty()) {
        std::cout << "Nie znaleziono klientów pasujących do frazy: " << keyword << std::endl;
        return;
    }

    std::cout << "\n=== Wyniki wyszukiwania ===\n";
    std::cout << "ID\tImię i nazwisko\tEmail\tTelefon\tData rejestracji\n";

    for (const auto& client : clients) {
        std::cout << client.getId() << "\t"
            << client.getFirstName() << " " << client.getLastName() << "\t"
            << client.getEmail() << "\t"
            << client.getPhone() << "\t"
            << client.getRegistrationDate() << "\n";
    }
}

// Funkcja do dodawania karnetu dla klienta
void addMembershipForClient(ClientService& clientService, MembershipService& membershipService) {
    int clientId;
    int membershipType;

    std::cout << "Podaj ID klienta: ";
    std::cin >> clientId;

    auto client = clientService.getClientById(clientId);
    if (!client) {
        std::cout << "Nie znaleziono klienta o ID: " << clientId << std::endl;
        return;
    }

    std::cout << "Wybierz rodzaj karnetu:\n";
    std::cout << "1. Miesięczny\n";
    std::cout << "2. Kwartalny\n";
    std::cout << "3. Roczny\n";
    std::cout << "Wybierz opcję: ";
    std::cin >> membershipType;

    bool isStudent = false;
    char studentChoice;
    std::cout << "Czy klient jest studentem? (T/N): ";
    std::cin >> studentChoice;

    if (studentChoice == 'T' || studentChoice == 't') {
        isStudent = true;
    }

    Membership membership;

    try {
        switch (membershipType) {
        case 1:
            membership = membershipService.createMonthlyMembership(clientId, isStudent);
            break;
        case 2:
            membership = membershipService.createQuarterlyMembership(clientId, isStudent);
            break;
        case 3:
            membership = membershipService.createYearlyMembership(clientId, isStudent);
            break;
        default:
            std::cout << "Nieprawidłowy wybór rodzaju karnetu." << std::endl;
            return;
        }

        int id = membershipService.addMembership(membership);
        std::cout << "Dodano nowy karnet z ID: " << id << std::endl;
        std::cout << "Termin ważności: od " << membership.getStartDate() << " do " << membership.getEndDate() << std::endl;
        std::cout << "Cena: " << membership.getPrice() << " zł" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
    }
}

// Funkcja do wyświetlania karnetów klienta
void displayMembershipsForClient(ClientService& clientService, MembershipService& membershipService) {
    int clientId;

    std::cout << "Podaj ID klienta: ";
    std::cin >> clientId;

    auto client = clientService.getClientById(clientId);
    if (!client) {
        std::cout << "Nie znaleziono klienta o ID: " << clientId << std::endl;
        return;
    }

    auto memberships = membershipService.getMembershipsByClientId(clientId);

    if (memberships.empty()) {
        std::cout << "Klient " << client->getFullName() << " nie posiada żadnych karnetów." << std::endl;
        return;
    }

    std::cout << "\n=== Karnety klienta " << client->getFullName() << " ===\n";
    std::cout << "ID\tRodzaj\tData rozpoczęcia\tData zakończenia\tCena\tAktywny\tWażny\tDni pozostałe\n";

    for (const auto& membership : memberships) {
        std::cout << membership.getId() << "\t"
            << membership.getType() << "\t"
            << membership.getStartDate() << "\t"
            << membership.getEndDate() << "\t"
            << membership.getPrice() << " zł\t"
            << (membership.getIsActive() ? "Tak" : "Nie") << "\t"
            << (membership.isValid() ? "Tak" : "Nie") << "\t"
            << membership.daysLeft() << "\n";
    }
}

// Funkcja do dodawania zajęć grupowych
void addGroupClass(ClassService& classService) {
    std::string name, trainer, date, time, description;
    int maxParticipants, duration;

    std::cin.ignore();
    std::cout << "Podaj nazwę zajęć: ";
    std::getline(std::cin, name);

    std::cout << "Podaj nazwisko trenera: ";
    std::getline(std::cin, trainer);

    std::cout << "Podaj maksymalną liczbę uczestników: ";
    std::cin >> maxParticipants;

    std::cin.ignore();
    std::cout << "Podaj datę zajęć (YYYY-MM-DD): ";
    std::getline(std::cin, date);

    std::cout << "Podaj godzinę zajęć (HH:MM): ";
    std::getline(std::cin, time);

    std::cout << "Podaj czas trwania zajęć (w minutach): ";
    std::cin >> duration;

    std::cin.ignore();
    std::cout << "Podaj opis zajęć: ";
    std::getline(std::cin, description);

    GymClass gymClass(-1, name, trainer, maxParticipants, date, time, duration, description);

    try {
        int id = classService.addClass(gymClass);
        std::cout << "Dodano nowe zajęcia grupowe z ID: " << id << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
    }
}

// Funkcja do wyświetlania wszystkich zajęć
void displayAllClasses(ClassService& classService) {
    auto classes = classService.getAllClasses();

    if (classes.empty()) {
        std::cout << "Brak zajęć w bazie danych." << std::endl;
        return;
    }

    std::cout << "\n=== Lista zajęć ===\n";
    std::cout << "ID\tNazwa\tTrener\tData\tGodzina\tCzas trwania\tMiejsca\tDostępne miejsca\n";

    for (const auto& gymClass : classes) {
        int availableSpots = classService.getAvailableSpotsForClass(gymClass.getId());

        std::cout << gymClass.getId() << "\t"
            << gymClass.getName() << "\t"
            << gymClass.getTrainer() << "\t"
            << gymClass.getDate() << "\t"
            << gymClass.getTime() << "\t"
            << gymClass.getDuration() << " min\t"
            << gymClass.getMaxParticipants() << "\t"
            << availableSpots << "\n";
    }
}

// Funkcja do rezerwacji miejsca na zajęciach
void reserveSpotForClass(ClientService& clientService, ClassService& classService) {
    int clientId, classId;

    std::cout << "Podaj ID klienta: ";
    std::cin >> clientId;

    auto client = clientService.getClientById(clientId);
    if (!client) {
        std::cout << "Nie znaleziono klienta o ID: " << clientId << std::endl;
        return;
    }

    std::cout << "Podaj ID zajęć: ";
    std::cin >> classId;

    auto gymClass = classService.getClassById(classId);
    if (!gymClass) {
        std::cout << "Nie znaleziono zajęć o ID: " << classId << std::endl;
        return;
    }

    int availableSpots = classService.getAvailableSpotsForClass(classId);
    if (availableSpots <= 0) {
        std::cout << "Brak dostępnych miejsc na tych zajęciach." << std::endl;
        return;
    }

    bool eligible = classService.isClientEligibleForClass(clientId, classId);
    if (!eligible) {
        std::cout << "Klient nie ma aktywnego karnetu lub nie spełnia innych warunków do zapisu na zajęcia." << std::endl;
        return;
    }

    Reservation reservation(-1, clientId, classId, "", "confirmed");

    try {
        int id = classService.addReservation(reservation);
        std::cout << "Dodano rezerwację z ID: " << id << std::endl;
        std::cout << "Klient " << client->getFullName() << " został zapisany na zajęcia " << gymClass->getName() << " w dniu " << gymClass->getDate() << " o godzinie " << gymClass->getTime() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
    }
}

// Funkcja do generowania raportów aktywności
void generateActivityReport(ReportService& reportService) {
    int reportType;

    std::cout << "\n=== Generowanie raportu aktywności ===\n";
    std::cout << "1. Raport aktywności klientów\n";
    std::cout << "2. Raport popularności zajęć\n";
    std::cout << "3. Raport finansowy\n";
    std::cout << "Wybierz rodzaj raportu: ";
    std::cin >> reportType;

    std::string startDate, endDate;
    std::cin.ignore();
    std::cout << "Podaj datę początkową (YYYY-MM-DD): ";
    std::getline(std::cin, startDate);

    std::cout << "Podaj datę końcową (YYYY-MM-DD): ";
    std::getline(std::cin, endDate);

    try {
        switch (reportType) {
        case 1: {
            auto reports = reportService.getClientActivityReport(startDate, endDate);
            std::cout << "\n=== Raport aktywności klientów ===\n";
            if (reports.empty()) {
                std::cout << "Brak danych dla podanego okresu." << std::endl;
                return;
            }

            std::cout << "Klient\tZajęcia ogółem\tZajęcia anulowane\tNajczęstsze zajęcia\tOstatnia wizyta\n";
            for (const auto& report : reports) {
                std::cout << report.clientName << "\t"
                    << report.totalClasses << "\t"
                    << report.cancelledClasses << "\t"
                    << report.mostFrequentClass << "\t"
                    << report.lastVisit << "\n";
            }
            break;
        }
        case 2: {
            auto reports = reportService.getClassPopularityReport(startDate, endDate);
            std::cout << "\n=== Raport popularności zajęć ===\n";
            if (reports.empty()) {
                std::cout << "Brak danych dla podanego okresu." << std::endl;
                return;
            }

            std::cout << "Zajęcia\tTrener\tRezerwacje\tWypełnienie %\n";
            for (const auto& report : reports) {
                std::cout << report.className << "\t"
                    << report.trainer << "\t"
                    << report.totalReservations << "\t"
                    << report.fillRate << "%\n";
            }
            break;
        }
        case 3: {
            auto report = reportService.getFinancialReport(startDate, endDate);
            std::cout << "\n=== Raport finansowy ===\n";
            std::cout << "Całkowity przychód: " << report.totalRevenue << " zł\n";
            std::cout << "Liczba sprzedanych karnetów: " << report.totalMemberships << "\n";
            std::cout << "Przychód z karnetów studenckich: " << report.studentRevenue << " zł\n";
            std::cout << "Przychód z karnetów standardowych: " << report.standardRevenue << " zł\n";

            std::cout << "\nPrzychody według typów karnetów:\n";
            for (const auto& [type, revenue] : report.revenueByMembershipType) {
                std::cout << type << ": " << revenue << " zł\n";
            }

            std::cout << "\nLiczba sprzedanych karnetów według typów:\n";
            for (const auto& [type, count] : report.membershipCountByType) {
                std::cout << type << ": " << count << "\n";
            }
            break;
        }
        default:
            std::cout << "Nieprawidłowy wybór rodzaju raportu." << std::endl;
            break;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
    }
}

int main() {
    // Ustawienie kodowania UTF-8 dla konsoli
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Ustaw locale dla poprawnego parsowania danych
    std::setlocale(LC_ALL, "pl_PL.UTF-8");
    try {
        // Inicjalizacja bazy danych
        DBManager dbManager("silka_system.db");
        dbManager.open();

        // Inicjalizacja DAO
        ClientDAO clientDao(dbManager);
        MembershipDAO membershipDao(dbManager);
        ClassDAO classDao(dbManager);

        // Inicjalizacja usług
        ClientService clientService(clientDao);
        MembershipService membershipService(membershipDao);
        ClassService classService(classDao, membershipService);
        ReportService reportService(dbManager);

        // Inicjalizacja klas importu/eksportu
        DataImport dataImport(clientService, membershipService, classService);
        DataExport dataExport(clientService, membershipService, classService);

        int choice;

        do {
            displayMenu();
            std::cin >> choice;

            switch (choice) {
            case 1:
                addNewClient(clientService);
                break;
            case 2:
                displayAllClients(clientService);
                break;
            case 3:
                searchClients(clientService);
                break;
            case 4:
                addMembershipForClient(clientService, membershipService);
                break;
            case 5:
                displayMembershipsForClient(clientService, membershipService);
                break;
            case 6:
                addGroupClass(classService);
                break;
            case 7:
                displayAllClasses(classService);
                break;
            case 8:
                reserveSpotForClass(clientService, classService);
                break;
            case 9:
                generateActivityReport(reportService);
                break;
            case 10:
                removeClientById(clientService);
                break;
            case 11: {
                // Import danych z pliku CSV
                int importType;
                std::string filePath;

                std::cout << "\n=== Import danych z CSV ===\n";
                std::cout << "1. Import klientów\n";
                std::cout << "2. Import karnetów\n";
                std::cout << "3. Import zajęć\n";
                std::cout << "Wybierz typ danych do importu: ";
                std::cin >> importType;

                std::cin.ignore();
                std::cout << "Podaj ścieżkę do pliku CSV: ";
                std::getline(std::cin, filePath);

                try {
                    switch (importType) {
                    case 1: {
                        auto clients = dataImport.importClientsFromCSV(filePath);
                        dataImport.saveImportedClients(clients);
                        std::cout << "Zaimportowano " << clients.size() << " klientów." << std::endl;
                        break;
                    }
                    case 2: {
                        auto memberships = dataImport.importMembershipsFromCSV(filePath);
                        dataImport.saveImportedMemberships(memberships);
                        std::cout << "Zaimportowano " << memberships.size() << " karnetów." << std::endl;
                        break;
                    }
                    case 3: {
                        auto classes = dataImport.importClassesFromCSV(filePath);
                        dataImport.saveImportedClasses(classes);
                        std::cout << "Zaimportowano " << classes.size() << " zajęć." << std::endl;
                        break;
                    }
                    default:
                        std::cout << "Nieprawidłowy typ danych." << std::endl;
                    }
                }
                catch (const ImportException& e) {
                    std::cerr << "Błąd importu: " << e.what() << std::endl;
                }
                break;
            }
            
            case 12: {
                // Import danych z pliku JSON
                int importType;
                std::string filePath;

                std::cout << "\n=== Import danych z JSON ===\n";
                std::cout << "1. Import klientów\n";
                std::cout << "2. Import karnetów\n";
                std::cout << "3. Import zajęć\n";
                std::cout << "Wybierz typ danych do importu: ";
                std::cin >> importType;

                std::cin.ignore();
                std::cout << "Podaj ścieżkę do pliku JSON: ";
                std::getline(std::cin, filePath);

                try {
                    switch (importType) {
                    case 1: {
                        auto clients = dataImport.importClientsFromJSON(filePath);
                        dataImport.saveImportedClients(clients);
                        std::cout << "Zaimportowano " << clients.size() << " klientów." << std::endl;
                        break;
                    }
                    case 2: {
                        auto memberships = dataImport.importMembershipsFromJSON(filePath);
                        dataImport.saveImportedMemberships(memberships);
                        std::cout << "Zaimportowano " << memberships.size() << " karnetów." << std::endl;
                        break;
                    }
                    case 3: {
                        auto classes = dataImport.importClassesFromJSON(filePath);
                        dataImport.saveImportedClasses(classes);
                        std::cout << "Zaimportowano " << classes.size() << " zajęć." << std::endl;
                        break;
                    }
                    default:
                        std::cout << "Nieprawidłowy typ danych." << std::endl;
                    }
                }
                catch (const ImportException& e) {
                    std::cerr << "Błąd importu: " << e.what() << std::endl;
                }
                break;
            }
            case 13: {
                // Eksport danych do pliku CSV
                int exportType;
                std::string filePath;

                std::cout << "\n=== Eksport danych do CSV ===\n";
                std::cout << "1. Eksport klientów\n";
                std::cout << "2. Eksport karnetów\n";
                std::cout << "3. Eksport zajęć\n";
                std::cout << "4. Eksport rezerwacji\n";
                std::cout << "Wybierz typ danych do eksportu: ";
                std::cin >> exportType;

                std::cin.ignore();
                std::cout << "Podaj ścieżkę do pliku CSV: ";
                std::getline(std::cin, filePath);

                try {
                    bool success = false;

                    switch (exportType) {
                    case 1:
                        success = dataExport.exportClientsToCSV(filePath);
                        break;
                    case 2:
                        success = dataExport.exportMembershipsToCSV(filePath);
                        break;
                    case 3:
                        success = dataExport.exportClassesToCSV(filePath);
                        break;
                    case 4:
                        success = dataExport.exportReservationsToCSV(filePath);
                        break;
                    default:
                        std::cout << "Nieprawidłowy typ danych." << std::endl;
                        break;
                    }

                    if (success) {
                        std::cout << "Dane zostały wyeksportowane do pliku: " << filePath << std::endl;
                    }
                }
                catch (const ExportException& e) {
                    std::cerr << "Błąd eksportu: " << e.what() << std::endl;
                }
                break;
            }
            case 14: {
                // Eksport danych do pliku JSON
                int exportType;
                std::string filePath;

                std::cout << "\n=== Eksport danych do JSON ===\n";
                std::cout << "1. Eksport klientów\n";
                std::cout << "2. Eksport karnetów\n";
                std::cout << "3. Eksport zajęć\n";
                std::cout << "4. Eksport rezerwacji\n";
                std::cout << "Wybierz typ danych do eksportu: ";
                std::cin >> exportType;

                std::cin.ignore();
                std::cout << "Podaj ścieżkę do pliku JSON: ";
                std::getline(std::cin, filePath);

                try {
                    bool success = false;

                    switch (exportType) {
                    case 1:
                        success = dataExport.exportClientsToJSON(filePath);
                        break;
                    case 2:
                        success = dataExport.exportMembershipsToJSON(filePath);
                        break;
                    case 3:
                        success = dataExport.exportClassesToJSON(filePath);
                        break;
                    case 4:
                        success = dataExport.exportReservationsToJSON(filePath);
                        break;
                    default:
                        std::cout << "Nieprawidłowy typ danych." << std::endl;
                        break;
                    }

                    if (success) {
                        std::cout << "Dane zostały wyeksportowane do pliku: " << filePath << std::endl;
                    }
                }
                catch (const ExportException& e) {
                    std::cerr << "Błąd eksportu: " << e.what() << std::endl;
                }
                break;
            }
            case 0:
                std::cout << "Dziękujemy za skorzystanie z systemu!" << std::endl;
                break;
            default:
                std::cout << "Nieprawidłowa opcja. Spróbuj ponownie." << std::endl;
            }
        } while (choice != 0);

        // Zamknięcie bazy danych
        dbManager.close();

    }
    catch (const DatabaseException& e) {
        std::cerr << "Błąd bazy danych: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}