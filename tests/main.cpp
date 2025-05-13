// tests/main.cpp
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <windows.h>
#include <limits>


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

void usunKlientaPoId(ClientService& uslugiKlienta) {
    int idKlienta;

    std::cout << "Podaj ID klienta do usunięcia: ";
    std::cin >> idKlienta;

    auto klient = uslugiKlienta.getClientById(idKlienta);
    if (!klient) {
        std::cout << "Nie znaleziono klienta o ID: " << idKlienta << std::endl;
        return;
    }

    std::cout << "Czy na pewno chcesz usunąć klienta " << klient->getFullName() << " (ID: " << idKlienta << ")? (T/N): ";
    char potwierdzenie;
    std::cin >> potwierdzenie;

    if (potwierdzenie == 'T' || potwierdzenie == 't') {
        bool sukces = uslugiKlienta.removeClient(idKlienta);

        if (sukces) {
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
void wyswietlMenu() {
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
void dodajNowegoKlienta(ClientService& uslugiKlienta) {
    std::cout << "\n=== Dodawanie nowego klienta ===\n";

    std::string imie, nazwisko, email, telefon, dataUrodzenia, uwagi;

    // Czyszczenie bufora przed rozpoczęciem wprowadzania danych
    std::cin.ignore(1000, '\n');

    // Pobieranie danych od użytkownika z walidacją
    bool poprawne = false;
    while (!poprawne) {
        std::cout << "Podaj imię: ";
        std::getline(std::cin, imie);
        if (imie.empty()) {
            std::cout << "Imię nie może być puste. Spróbuj ponownie.\n";
        }
        else {
            poprawne = true;
        }
    }

    poprawne = false;
    while (!poprawne) {
        std::cout << "Podaj nazwisko: ";
        std::getline(std::cin, nazwisko);
        if (nazwisko.empty()) {
            std::cout << "Nazwisko nie może być puste. Spróbuj ponownie.\n";
        }
        else {
            poprawne = true;
        }
    }

    std::cout << "Podaj email: ";
    std::getline(std::cin, email);

    std::cout << "Podaj telefon: ";
    std::getline(std::cin, telefon);

    std::cout << "Podaj datę urodzenia (RRRR-MM-DD): ";
    std::getline(std::cin, dataUrodzenia);

    std::cout << "Podaj uwagi: ";
    std::getline(std::cin, uwagi);

    // Podsumowanie wprowadzonych danych przed dodaniem
    std::cout << "\nPodsumowanie wprowadzonych danych:\n";
    std::cout << "Imię: [" << imie << "]\n";
    std::cout << "Nazwisko: [" << nazwisko << "]\n";
    std::cout << "Email: [" << email << "]\n";
    std::cout << "Telefon: [" << telefon << "]\n";
    std::cout << "Data urodzenia: [" << dataUrodzenia << "]\n";
    std::cout << "Uwagi: [" << uwagi << "]\n";

    std::cout << "\nCzy dane są poprawne? (T/N): ";
    char potwierdzenie;
    std::cin >> potwierdzenie;

    if (potwierdzenie == 'T' || potwierdzenie == 't') {
        Client klient(-1, imie, nazwisko, email, telefon, dataUrodzenia, "", uwagi);

        try {
            int id = uslugiKlienta.addClient(klient);
            std::cout << "Dodano nowego klienta z ID: " << id << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Błąd dodawania klienta: " << e.what() << std::endl;
        }
    }
    else {
        std::cout << "Anulowano dodawanie klienta.\n";
    }
}

// Funkcja do wyświetlania wszystkich klientów
void wyswietlWszystkichKlientow(ClientService& uslugiKlienta) {
    auto klienci = uslugiKlienta.getAllClients();

    if (klienci.empty()) {
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

    for (const auto& klient : klienci) {
        std::cout << std::left
            << std::setw(5) << klient.getId()
            << std::setw(30) << klient.getFirstName() + " " + klient.getLastName()
            << std::setw(30) << klient.getEmail()
            << std::setw(15) << klient.getPhone()
            << std::setw(15) << klient.getRegistrationDate()
            << std::endl;
    }
}

// Funkcja do wyszukiwania klientów
void wyszukajKlientow(ClientService& uslugiKlienta) {
    std::string fraza;
    std::cin.ignore();
    std::cout << "Podaj frazę do wyszukania: ";
    std::getline(std::cin, fraza);

    auto klienci = uslugiKlienta.searchClients(fraza);

    if (klienci.empty()) {
        std::cout << "Nie znaleziono klientów pasujących do frazy: " << fraza << std::endl;
        return;
    }

    std::cout << "\n=== Wyniki wyszukiwania ===\n";
    std::cout << "ID\tImię i nazwisko\tEmail\tTelefon\tData rejestracji\n";

    for (const auto& klient : klienci) {
        std::cout << klient.getId() << "\t"
            << klient.getFirstName() << " " << klient.getLastName() << "\t"
            << klient.getEmail() << "\t"
            << klient.getPhone() << "\t"
            << klient.getRegistrationDate() << "\n";
    }
}

// Funkcja do dodawania karnetu dla klienta
void dodajKarnetDlaKlienta(ClientService& uslugiKlienta, MembershipService& uslugiKarnetu) {
    int idKlienta;
    int typKarnetu;

    std::cout << "Podaj ID klienta: ";
    std::cin >> idKlienta;

    auto klient = uslugiKlienta.getClientById(idKlienta);
    if (!klient) {
        std::cout << "Nie znaleziono klienta o ID: " << idKlienta << std::endl;
        return;
    }

    std::cout << "Wybierz rodzaj karnetu:\n";
    std::cout << "1. Miesięczny\n";
    std::cout << "2. Kwartalny\n";
    std::cout << "3. Roczny\n";
    std::cout << "Wybierz opcję: ";
    std::cin >> typKarnetu;

    bool czyStudent = false;
    char wyborStudenta;
    std::cout << "Czy klient jest studentem? (T/N): ";
    std::cin >> wyborStudenta;

    if (wyborStudenta == 'T' || wyborStudenta == 't') {
        czyStudent = true;
    }

    Membership karnet;
    std::string nazwaTypuKarnetu;

    try {
        switch (typKarnetu) {
        case 1:
            karnet = uslugiKarnetu.createMonthlyMembership(idKlienta, czyStudent);
            nazwaTypuKarnetu = czyStudent ? "student_miesieczny" : "normalny_miesieczny";
            break;
        case 2:
            karnet = uslugiKarnetu.createQuarterlyMembership(idKlienta, czyStudent);
            nazwaTypuKarnetu = czyStudent ? "student_kwartalny" : "normalny_kwartalny";
            break;
        case 3:
            karnet = uslugiKarnetu.createYearlyMembership(idKlienta, czyStudent);
            nazwaTypuKarnetu = czyStudent ? "student_roczny" : "normalny_roczny";
            break;
        default:
            std::cout << "Nieprawidłowy wybór rodzaju karnetu." << std::endl;
            return;
        }

        // Upewnij się, że typ jest ustawiony
        karnet.setType(nazwaTypuKarnetu);

        // Debug - pokaż dane przed dodaniem
        std::cout << "DEBUG - Dane karnetu przed dodaniem:\n";
        std::cout << "  ClientID: " << karnet.getClientId() << "\n";
        std::cout << "  Type: [" << karnet.getType() << "]\n";
        std::cout << "  StartDate: [" << karnet.getStartDate() << "]\n";
        std::cout << "  EndDate: [" << karnet.getEndDate() << "]\n";
        std::cout << "  Price: [" << karnet.getPrice() << "]\n";

        int id = uslugiKarnetu.addMembership(karnet);
        std::cout << "Dodano nowy karnet z ID: " << id << std::endl;
        std::cout << "Termin ważności: od " << karnet.getStartDate() << " do " << karnet.getEndDate() << std::endl;
        std::cout << "Cena: " << karnet.getPrice() << " zł" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
    }
}