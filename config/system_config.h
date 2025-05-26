// config/system_config.h
#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include <string>
#include <map>

struct SystemConfig {
    // Konfiguracja bazy danych
    std::string databasePath = "silownia.db";
    int maxConnections = 10;
    bool enableLogging = true;
    
    // Konfiguracja raportów
    std::string defaultReportPath = "raporty/";
    std::string defaultFormat = "HTML";
    bool autoBackupReports = true;
    
    // Konfiguracja historii
    int maxHistoryDays = 90;        // Dni przechowywania historii
    int maxHistoryOperations = 1000; // Max operacji w pamięci
    bool autoCleanup = true;         // Automatyczne czyszczenie
    
    // Konfiguracja karnetów - ceny domyślne
    struct PriceConfig {
        double normalny_miesieczny = 120.0;
        double student_miesieczny = 80.0;
        double normalny_kwartalny = 300.0;
        double student_kwartalny = 200.0;
        double normalny_roczny = 1000.0;
        double student_roczny = 600.0;
    } ceny;
    
    // Konfiguracja interfejsu
    std::string language = "pl";     // pl, en
    bool colorOutput = true;         // Kolorowe wyjście w konsoli
    int pageSize = 20;              // Ilość rekordów na stronę
    
    // Automatyczne punkty przywracania
    bool autoRestorePoints = true;
    int autoRestoreInterval = 60;    // minuty między automatycznymi punktami
    
    // Konfiguracja alertów
    int karnetExpiryWarningDays = 7; // Ostrzeżenie X dni przed wygaśnięciem
    bool emailNotifications = false;
    std::string smtpServer = "";
    std::string emailFrom = "";
    
    // Metody do wczytywania/zapisywania konfiguracji
    bool wczytajZPliku(const std::string& sciezka = "config.ini");
    bool zapiszDoPliku(const std::string& sciezka = "config.ini");
    void ustawDomyslne();
    
    // Walidacja konfiguracji
    bool sprawdzPoprawnosc() const;
    std::string pobierzBleady() const;
};

// utils/config_manager.cpp
#include "system_config.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool SystemConfig::wczytajZPliku(const std::string& sciezka) {
    std::ifstream plik(sciezka);
    if (!plik.is_open()) {
        std::cout << "Nie można otworzyć pliku konfiguracji: " << sciezka << std::endl;
        std::cout << "Używam konfiguracji domyślnej." << std::endl;
        ustawDomyslne();
        return zapiszDoPliku(sciezka); // Utwórz plik z domyślnymi ustawieniami
    }
    
    std::string linia;
    std::string sekcja = "";
    
    while (std::getline(plik, linia)) {
        // Usuń białe znaki
        linia.erase(0, linia.find_first_not_of(" \t"));
        linia.erase(linia.find_last_not_of(" \t") + 1);
        
        // Ignoruj puste linie i komentarze
        if (linia.empty() || linia[0] == '#' || linia[0] == ';') {
            continue;
        }
        
        // Sekcje
        if (linia[0] == '[' && linia.back() == ']') {
            sekcja = linia.substr(1, linia.length() - 2);
            continue;
        }
        
        // Para klucz=wartość
        size_t pos = linia.find('=');
        if (pos == std::string::npos) continue;
        
        std::string klucz = linia.substr(0, pos);
        std::string wartosc = linia.substr(pos + 1);
        
        // Usuń białe znaki z klucza i wartości
        klucz.erase(klucz.find_last_not_of(" \t") + 1);
        wartosc.erase(0, wartosc.find_first_not_of(" \t"));
        
        // Parsowanie według sekcji
        if (sekcja == "Database") {
            if (klucz == "path") databasePath = wartosc;
            else if (klucz == "maxConnections") maxConnections = std::stoi(wartosc);
            else if (klucz == "enableLogging") enableLogging = (wartosc == "true");
        }
        else if (sekcja == "Reports") {
            if (klucz == "defaultPath") defaultReportPath = wartosc;
            else if (klucz == "defaultFormat") defaultFormat = wartosc;
            else if (klucz == "autoBackup") autoBackupReports = (wartosc == "true");
        }
        else if (sekcja == "History") {
            if (klucz == "maxDays") maxHistoryDays = std::stoi(wartosc);
            else if (klucz == "maxOperations") maxHistoryOperations = std::stoi(wartosc);
            else if (klucz == "autoCleanup") autoCleanup = (wartosc == "true");
        }
        else if (sekcja == "Prices") {
            if (klucz == "normalny_miesieczny") ceny.normalny_miesieczny = std::stod(wartosc);
            else if (klucz == "student_miesieczny") ceny.student_miesieczny = std::stod(wartosc);
            else if (klucz == "normalny_kwartalny") ceny.normalny_kwartalny = std::stod(wartosc);
            else if (klucz == "student_kwartalny") ceny.student_kwartalny = std::stod(wartosc);
            else if (klucz == "normalny_roczny") ceny.normalny_roczny = std::stod(wartosc);
            else if (klucz == "student_roczny") ceny.student_roczny = std::stod(wartosc);
        }
        else if (sekcja == "Interface") {
            if (klucz == "language") language = wartosc;
            else if (klucz == "colorOutput") colorOutput = (wartosc == "true");
            else if (klucz == "pageSize") pageSize = std::stoi(wartosc);
        }
        else if (sekcja == "Alerts") {
            if (klucz == "karnetExpiryWarningDays") karnetExpiryWarningDays = std::stoi(wartosc);
            else if (klucz == "emailNotifications") emailNotifications = (wartosc == "true");
            else if (klucz == "smtpServer") smtpServer = wartosc;
            else if (klucz == "emailFrom") emailFrom = wartosc;
        }
    }
    
    plik.close();
    return sprawdzPoprawnosc();
}

bool SystemConfig::zapiszDoPliku(const std::string& sciezka) {
    std::ofstream plik(sciezka);
    if (!plik.is_open()) {
        std::cerr << "Nie można zapisać pliku konfiguracji: " << sciezka << std::endl;
        return false;
    }
    
    plik << "# Konfiguracja Systemu Zarządzania Siłownią\n";
    plik << "# Generowany automatycznie - " << __DATE__ << " " << __TIME__ << "\n\n";
    
    plik << "[Database]\n";
    plik << "path=" << databasePath << "\n";
    plik << "maxConnections=" << maxConnections << "\n";
    plik << "enableLogging=" << (enableLogging ? "true" : "false") << "\n\n";
    
    plik << "[Reports]\n";
    plik << "defaultPath=" << defaultReportPath << "\n";
    plik << "defaultFormat=" << defaultFormat << "\n";
    plik << "autoBackup=" << (autoBackupReports ? "true" : "false") << "\n\n";
    
    plik << "[History]\n";
    plik << "maxDays=" << maxHistoryDays << "\n";
    plik << "maxOperations=" << maxHistoryOperations << "\n";
    plik << "autoCleanup=" << (autoCleanup ? "true" : "false") << "\n\n";
    
    plik << "[Prices]\n";
    plik << "# Ceny karnetów w złotych\n";
    plik << "normalny_miesieczny=" << ceny.normalny_miesieczny << "\n";
    plik << "student_miesieczny=" << ceny.student_miesieczny << "\n";
    plik << "normalny_kwartalny=" << ceny.normalny_kwartalny << "\n";
    plik << "student_kwartalny=" << ceny.student_kwartalny << "\n";
    plik << "normalny_roczny=" << ceny.normalny_roczny << "\n";
    plik << "student_roczny=" << ceny.student_roczny << "\n\n";
    
    plik << "[Interface]\n";
    plik << "language=" << language << "\n";
    plik << "colorOutput=" << (colorOutput ? "true" : "false") << "\n";
    plik << "pageSize=" << pageSize << "\n\n";
    
    plik << "[Alerts]\n";
    plik << "karnetExpiryWarningDays=" << karnetExpiryWarningDays << "\n";
    plik << "emailNotifications=" << (emailNotifications ? "true" : "false") << "\n";
    plik << "smtpServer=" << smtpServer << "\n";
    plik << "emailFrom=" << emailFrom << "\n\n";
    
    plik << "# Koniec konfiguracji\n";
    plik.close();
    
    std::cout << "Zapisano konfigurację do pliku: " << sciezka << std::endl;
    return true;
}

void SystemConfig::ustawDomyslne() {
    // Wartości są już ustawione w definicji struktury
    std::cout << "Ustawiono konfigurację domyślną." << std::endl;
}

bool SystemConfig::sprawdzPoprawnosc() const {
    // Sprawdzenie poprawności ścieżek i wartości
    if (databasePath.empty()) return false;
    if (maxConnections <= 0) return false;
    if (maxHistoryDays <= 0) return false;
    if (pageSize <= 0) return false;
    
    // Sprawdzenie cen
    if (ceny.normalny_miesieczny <= 0) return false;
    if (ceny.student_miesieczny <= 0) return false;
    
    return true;
}

std::string SystemConfig::pobierzBleady() const {
    std::stringstream bledy;
    
    if (databasePath.empty()) {
        bledy << "- Ścieżka do bazy danych nie może być pusta\n";
    }
    
    if (maxConnections <= 0) {
        bledy << "- Maksymalna liczba połączeń musi być większa od 0\n";
    }
    
    if (maxHistoryDays <= 0) {
        bledy << "- Czas przechowywania historii musi być większy od 0\n";
    }
    
    if (ceny.normalny_miesieczny <= 0) {
        bledy << "- Cena karnetu miesięcznego musi być większa od 0\n";
    }
    
    return bledy.str();
}

#endif // SYSTEM_CONFIG_H