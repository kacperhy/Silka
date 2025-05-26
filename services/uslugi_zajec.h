// services/uslugi_zajec.h
#ifndef USLUGI_ZAJEC_H
#define USLUGI_ZAJEC_H

#include <vector>
#include <memory>
#include "../models/zajecia.h"
#include "../models/rezerwacja.h"
#include "../database/dao/zajecia_dao.h"
#include "uslugi_karnetu.h"

class UslugiZajec {
public:
    UslugiZajec(ZajeciaDAO& zajeciaDAO, UslugiKarnetu& uslugiKarnetu);

    std::vector<Zajecia> pobierzWszystkieZajecia();
    std::unique_ptr<Zajecia> pobierzZajeciaPoId(int id);
    int dodajZajecia(const Zajecia& zajecia);
    bool aktualizujZajecia(const Zajecia& zajecia);
    bool usunZajecia(int id);

    std::vector<Rezerwacja> pobierzWszystkieRezerwacje();
    std::vector<Rezerwacja> pobierzRezerwacjeKlienta(int idKlienta);
    std::vector<Rezerwacja> pobierzRezerwacjeZajec(int idZajec);
    int dodajRezerwacje(const Rezerwacja& rezerwacja);
    bool anulujRezerwacje(int idRezerwacji);

    int pobierzDostepneMiejscaZajec(int idZajec);
    bool czyKlientUprawniony(int idKlienta, int idZajec);

private:
    ZajeciaDAO& zajeciaDAO;
    UslugiKarnetu& uslugiKarnetu;
};
// utils/config_manager.cpp
#include "system_config.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool SystemConfig::wczytajZPliku(const std::string& sciezka) {
    std::ifstream plik(sciezka);
    if (!plik.is_open()) {
        std::cout << "Nie mo¿na otworzyæ pliku konfiguracji: " << sciezka << std::endl;
        std::cout << "U¿ywam konfiguracji domyœlnej." << std::endl;
        ustawDomyslne();
        return zapiszDoPliku(sciezka); // Utwórz plik z domyœlnymi ustawieniami
    }
    
    std::string linia;
    std::string sekcja = "";
    
    while (std::getline(plik, linia)) {
        // Usuñ bia³e znaki
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
        
        // Para klucz=wartoœæ
        size_t pos = linia.find('=');
        if (pos == std::string::npos) continue;
        
        std::string klucz = linia.substr(0, pos);
        std::string wartosc = linia.substr(pos + 1);
        
        // Usuñ bia³e znaki z klucza i wartoœci
        klucz.erase(klucz.find_last_not_of(" \t") + 1);
        wartosc.erase(0, wartosc.find_first_not_of(" \t"));
        
        // Parsowanie wed³ug sekcji
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
        std::cerr << "Nie mo¿na zapisaæ pliku konfiguracji: " << sciezka << std::endl;
        return false;
    }
    
    plik << "# Konfiguracja Systemu Zarz¹dzania Si³owni¹\n";
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
    plik << "# Ceny karnetów w z³otych\n";
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
    
    std::cout << "Zapisano konfiguracjê do pliku: " << sciezka << std::endl;
    return true;
}

void SystemConfig::ustawDomyslne() {
    // Wartoœci s¹ ju¿ ustawione w definicji struktury
    std::cout << "Ustawiono konfiguracjê domyœln¹." << std::endl;
}

bool SystemConfig::sprawdzPoprawnosc() const {
    // Sprawdzenie poprawnoœci œcie¿ek i wartoœci
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
        bledy << "- Œcie¿ka do bazy danych nie mo¿e byæ pusta\n";
    }
    
    if (maxConnections <= 0) {
        bledy << "- Maksymalna liczba po³¹czeñ musi byæ wiêksza od 0\n";
    }
    
    if (maxHistoryDays <= 0) {
        bledy << "- Czas przechowywania historii musi byæ wiêkszy od 0\n";
    }
    
    if (ceny.normalny_miesieczny <= 0) {
        bledy << "- Cena karnetu miesiêcznego musi byæ wiêksza od 0\n";
    }
    
    return bledy.str();
}
// utils/config_manager.cpp
#include "system_config.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool SystemConfig::wczytajZPliku(const std::string& sciezka) {
    std::ifstream plik(sciezka);
    if (!plik.is_open()) {
        std::cout << "Nie mo¿na otworzyæ pliku konfiguracji: " << sciezka << std::endl;
        std::cout << "U¿ywam konfiguracji domyœlnej." << std::endl;
        ustawDomyslne();
        return zapiszDoPliku(sciezka); // Utwórz plik z domyœlnymi ustawieniami
    }

    std::string linia;
    std::string sekcja = "";

    while (std::getline(plik, linia)) {
        // Usuñ bia³e znaki
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

        // Para klucz=wartoœæ
        size_t pos = linia.find('=');
        if (pos == std::string::npos) continue;

        std::string klucz = linia.substr(0, pos);
        std::string wartosc = linia.substr(pos + 1);

        // Usuñ bia³e znaki z klucza i wartoœci
        klucz.erase(klucz.find_last_not_of(" \t") + 1);
        wartosc.erase(0, wartosc.find_first_not_of(" \t"));

        // Parsowanie wed³ug sekcji
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
        std::cerr << "Nie mo¿na zapisaæ pliku konfiguracji: " << sciezka << std::endl;
        return false;
    }

    plik << "# Konfiguracja Systemu Zarz¹dzania Si³owni¹\n";
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
    plik << "# Ceny karnetów w z³otych\n";
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

    std::cout << "Zapisano konfiguracjê do pliku: " << sciezka << std::endl;
    return true;
}

void SystemConfig::ustawDomyslne() {
    // Wartoœci s¹ ju¿ ustawione w definicji struktury
    std::cout << "Ustawiono konfiguracjê domyœln¹." << std::endl;
}

bool SystemConfig::sprawdzPoprawnosc() const {
    // Sprawdzenie poprawnoœci œcie¿ek i wartoœci
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
        bledy << "- Œcie¿ka do bazy danych nie mo¿e byæ pusta\n";
    }

    if (maxConnections <= 0) {
        bledy << "- Maksymalna liczba po³¹czeñ musi byæ wiêksza od 0\n";
    }

    if (maxHistoryDays <= 0) {
        bledy << "- Czas przechowywania historii musi byæ wiêkszy od 0\n";
    }

    if (ceny.normalny_miesieczny <= 0) {
        bledy << "- Cena karnetu miesiêcznego musi byæ wiêksza od 0\n";
    }

    return bledy.str();
}
// utils/config_manager.cpp
#include "system_config.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool SystemConfig::wczytajZPliku(const std::string& sciezka) {
    std::ifstream plik(sciezka);
    if (!plik.is_open()) {
        std::cout << "Nie mo¿na otworzyæ pliku konfiguracji: " << sciezka << std::endl;
        std::cout << "U¿ywam konfiguracji domyœlnej." << std::endl;
        ustawDomyslne();
        return zapiszDoPliku(sciezka); // Utwórz plik z domyœlnymi ustawieniami
    }
    
    std::string linia;
    std::string sekcja = "";
    
    while (std::getline(plik, linia)) {
        // Usuñ bia³e znaki
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
        
        // Para klucz=wartoœæ
        size_t pos = linia.find('=');
        if (pos == std::string::npos) continue;
        
        std::string klucz = linia.substr(0, pos);
        std::string wartosc = linia.substr(pos + 1);
        
        // Usuñ bia³e znaki z klucza i wartoœci
        klucz.erase(klucz.find_last_not_of(" \t") + 1);
        wartosc.erase(0, wartosc.find_first_not_of(" \t"));
        
        // Parsowanie wed³ug sekcji
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
        std::cerr << "Nie mo¿na zapisaæ pliku konfiguracji: " << sciezka << std::endl;
        return false;
    }
    
    plik << "# Konfiguracja Systemu Zarz¹dzania Si³owni¹\n";
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
    plik << "# Ceny karnetów w z³otych\n";
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
    
    std::cout << "Zapisano konfiguracjê do pliku: " << sciezka << std::endl;
    return true;
}

void SystemConfig::ustawDomyslne() {
    // Wartoœci s¹ ju¿ ustawione w definicji struktury
    std::cout << "Ustawiono konfiguracjê domyœln¹." << std::endl;
}

bool SystemConfig::sprawdzPoprawnosc() const {
    // Sprawdzenie poprawnoœci œcie¿ek i wartoœci
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
        bledy << "- Œcie¿ka do bazy danych nie mo¿e byæ pusta\n";
    }
    
    if (maxConnections <= 0) {
        bledy << "- Maksymalna liczba po³¹czeñ musi byæ wiêksza od 0\n";
    }
    
    if (maxHistoryDays <= 0) {
        bledy << "- Czas przechowywania historii musi byæ wiêkszy od 0\n";
    }
    
    if (ceny.normalny_miesieczny <= 0) {
        bledy << "- Cena karnetu miesiêcznego musi byæ wiêksza od 0\n";
    }
    
    return bledy.str();
}

#endif 