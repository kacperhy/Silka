// Dodaj do istniejących #include:
#include "../utils/data_import.h"
#include "../utils/data_export.h"
#include <iostream>

// W funkcji displayMenu() dodaj nowe opcje:
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
    std::cout << "\n=== Import/Export danych ===\n";
    std::cout << "10. Import danych z pliku CSV\n";
    std::cout << "11. Import danych z pliku JSON\n";
    std::cout << "12. Eksport danych do pliku CSV\n";
    std::cout << "13. Eksport danych do pliku JSON\n";
    std::cout << "0. Wyjście\n";
    std::cout << "Wybierz opcję: ";
}

// W funkcji main() dodaj nowe przypadki w switch:
int main() {
    try {
        // ... istniejący kod ...
        
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
                // ... istniejące przypadki ...
                
                case 10: {
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
                    } catch (const ImportException& e) {
                        std::cerr << "Błąd importu: " << e.what() << std::endl;
                    }
                    break;
                }
                case 11: {
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
                    } catch (const ImportException& e) {
                        std::cerr << "Błąd importu: " << e.what() << std::endl;
                    }
                    break;
                }
                case 12: {
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
                    } catch (const ExportException& e) {
                        std::cerr << "Błąd eksportu: " << e.what() << std::endl;
                    }
                    break;
                }
                case 13: {
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
                    } catch (const ExportException& e) {
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
        
    } catch (const DatabaseException& e) {
        std::cerr << "Błąd bazy danych: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}