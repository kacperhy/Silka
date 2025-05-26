#define NOMINMAX
// tests/main.cpp
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <windows.h>
#include <limits>
#include <iomanip>

// Poprawne include'y zgodne z polską strukturą projektu
#include "../database/menedzer_bd.h"
#include "../database/dao/klient_dao.h" 
#include "../database/dao/karnet_dao.h"
#include "../services/uslugi_klienta.h"
#include "../services/uslugi_karnetu.h"
#include "../models/klient.h"
#include "../models/karnet.h"
#include "../models/zajecia.h"
#include "../models/rezerwacja.h"

// Funkcja wyświetlająca menu główne
void wyswietlMenu() {
    std::cout << "\n===== System zarządzania siłownią =====\n";
    std::cout << "=== KLIENCI ===\n";
    std::cout << "1. Dodaj nowego klienta\n";
    std::cout << "2. Wyświetl wszystkich klientów\n";
    std::cout << "3. Wyszukaj klienta\n";
    std::cout << "4. Usuń klienta\n";
    std::cout << "\n=== KARNETY ===\n";
    std::cout << "5. Dodaj karnet dla klienta\n";
    std::cout << "6. Wyświetl karnety klienta\n";
    std::cout << "7. Usuń karnet\n";
    std::cout << "\n=== ZAJĘCIA I REZERWACJE ===\n";
    std::cout << "8. Zarządzanie zajęciami (w przygotowaniu)\n";
    std::cout << "9. Zarządzanie rezerwacjami (w przygotowaniu)\n";
    std::cout << "\n=== RAPORTY ===\n";
    std::cout << "10. Wygeneruj raport aktywności (w przygotowaniu)\n";
    std::cout << "\n=== IMPORT/EXPORT ===\n";
    std::cout << "11. Import/Export danych (w przygotowaniu)\n";
    std::cout << "\n0. Wyjście\n";
    std::cout << "Wybierz opcję: ";
}

// === FUNKCJE DO ZARZĄDZANIA KLIENTAMI ===

void dodajNowegoKlienta(UslugiKlienta& uslugiKlienta) {
    std::cout << "\n=== Dodawanie nowego klienta ===\n";

    std::string imie, nazwisko, email, telefon, dataUrodzenia, uwagi;

    std::cin.ignore(1000, '\n');

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
        Klient klient(-1, imie, nazwisko, email, telefon, dataUrodzenia, "", uwagi);

        try {
            int id = uslugiKlienta.dodajKlienta(klient);
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

void wyswietlWszystkichKlientow(UslugiKlienta& uslugiKlienta) {
    auto klienci = uslugiKlienta.pobierzWszystkichKlientow();

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
            << std::setw(5) << klient.pobierzId()
            << std::setw(30) << klient.pobierzPelneNazwisko()
            << std::setw(30) << klient.pobierzEmail()
            << std::setw(15) << klient.pobierzTelefon()
            << std::setw(15) << klient.pobierzDateRejestracji()
            << std::endl;
    }
}

void wyszukajKlientow(UslugiKlienta& uslugiKlienta) {
    std::string fraza;
    std::cin.ignore();
    std::cout << "Podaj frazę do wyszukania: ";
    std::getline(std::cin, fraza);

    auto klienci = uslugiKlienta.wyszukajKlientow(fraza);

    if (klienci.empty()) {
        std::cout << "Nie znaleziono klientów pasujących do frazy: " << fraza << std::endl;
        return;
    }

    std::cout << "\n=== Wyniki wyszukiwania ===\n";
    std::cout << std::left
        << std::setw(5) << "ID"
        << std::setw(30) << "Imię i nazwisko"
        << std::setw(30) << "Email"
        << std::setw(15) << "Telefon"
        << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    for (const auto& klient : klienci) {
        std::cout << std::left
            << std::setw(5) << klient.pobierzId()
            << std::setw(30) << klient.pobierzPelneNazwisko()
            << std::setw(30) << klient.pobierzEmail()
            << std::setw(15) << klient.pobierzTelefon()
            << std::endl;
    }
}

void usunKlienta(UslugiKlienta& uslugiKlienta) {
    int idKlienta;

    std::cout << "Podaj ID klienta do usunięcia: ";
    std::cin >> idKlienta;

    auto klient = uslugiKlienta.pobierzKlientaPoId(idKlienta);
    if (!klient) {
        std::cout << "Nie znaleziono klienta o ID: " << idKlienta << std::endl;
        return;
    }

    std::cout << "Czy na pewno chcesz usunąć klienta " << klient->pobierzPelneNazwisko()
        << " (ID: " << idKlienta << ")? (T/N): ";
    char potwierdzenie;
    std::cin >> potwierdzenie;

    if (potwierdzenie == 'T' || potwierdzenie == 't') {
        bool sukces = uslugiKlienta.usunKlienta(idKlienta);

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

// === FUNKCJE DO ZARZĄDZANIA KARNETAMI ===

void dodajKarnetDlaKlienta(UslugiKlienta& uslugiKlienta, UslugiKarnetu& uslugiKarnetu) {
    int idKlienta;
    int typKarnetu;

    std::cout << "Podaj ID klienta: ";
    std::cin >> idKlienta;

    auto klient = uslugiKlienta.pobierzKlientaPoId(idKlienta);
    if (!klient) {
        std::cout << "Nie znaleziono klienta o ID: " << idKlienta << std::endl;
        return;
    }

    std::cout << "Klient: " << klient->pobierzPelneNazwisko() << std::endl;

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

    try {
        Karnet karnet;
        karnet.ustawIdKlienta(idKlienta);

        switch (typKarnetu) {
        case 1: {
            karnet.ustawTyp(czyStudent ? "student_miesieczny" : "normalny_miesieczny");
            karnet.ustawCene(czyStudent ? 80.0 : 120.0);
            karnet.ustawDateRozpoczecia(Karnet::pobierzAktualnaDate());
            karnet.ustawDateZakonczenia(Karnet::dodajDniDoData(Karnet::pobierzAktualnaDate(), 30));
            break;
        }
        case 2: {
            karnet.ustawTyp(czyStudent ? "student_kwartalny" : "normalny_kwartalny");
            karnet.ustawCene(czyStudent ? 200.0 : 300.0);
            karnet.ustawDateRozpoczecia(Karnet::pobierzAktualnaDate());
            karnet.ustawDateZakonczenia(Karnet::dodajDniDoData(Karnet::pobierzAktualnaDate(), 90));
            break;
        }
        case 3: {
            karnet.ustawTyp(czyStudent ? "student_roczny" : "normalny_roczny");
            karnet.ustawCene(czyStudent ? 600.0 : 1000.0);
            karnet.ustawDateRozpoczecia(Karnet::pobierzAktualnaDate());
            karnet.ustawDateZakonczenia(Karnet::dodajDniDoData(Karnet::pobierzAktualnaDate(), 365));
            break;
        }
        default:
            std::cout << "Nieprawidłowy wybór rodzaju karnetu." << std::endl;
            return;
        }

        karnet.ustawCzyAktywny(true);

        int id = uslugiKarnetu.dodajKarnet(karnet);
        std::cout << "Dodano nowy karnet z ID: " << id << std::endl;
        std::cout << "Termin ważności: od " << karnet.pobierzDateRozpoczecia()
            << " do " << karnet.pobierzDateZakonczenia() << std::endl;
        std::cout << "Cena: " << karnet.pobierzCene() << " zł" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
    }
}

void wyswietlKarnetyKlienta(UslugiKlienta& uslugiKlienta, UslugiKarnetu& uslugiKarnetu) {
    int idKlienta;

    std::cout << "Podaj ID klienta: ";
    std::cin >> idKlienta;

    auto klient = uslugiKlienta.pobierzKlientaPoId(idKlienta);
    if (!klient) {
        std::cout << "Nie znaleziono klienta o ID: " << idKlienta << std::endl;
        return;
    }

    auto karnety = uslugiKarnetu.pobierzKarnetyKlienta(idKlienta);

    if (karnety.empty()) {
        std::cout << "Klient nie ma żadnych karnetów." << std::endl;
        return;
    }

    std::cout << "\n=== Karnety klienta: " << klient->pobierzPelneNazwisko() << " ===\n";
    std::cout << std::left
        << std::setw(5) << "ID"
        << std::setw(20) << "Typ"
        << std::setw(15) << "Od"
        << std::setw(15) << "Do"
        << std::setw(10) << "Cena"
        << std::setw(10) << "Aktywny"
        << std::endl;
    std::cout << std::string(75, '-') << std::endl;

    for (const auto& karnet : karnety) {
        std::cout << std::left
            << std::setw(5) << karnet.pobierzId()
            << std::setw(20) << karnet.pobierzTyp()
            << std::setw(15) << karnet.pobierzDateRozpoczecia()
            << std::setw(15) << karnet.pobierzDateZakonczenia()
            << std::setw(10) << karnet.pobierzCene()
            << std::setw(10) << (karnet.pobierzCzyAktywny() ? "Tak" : "Nie")
            << std::endl;
    }
}

void usunKarnet(UslugiKarnetu& uslugiKarnetu) {
    int idKarnetu;

    std::cout << "Podaj ID karnetu do usunięcia: ";
    std::cin >> idKarnetu;

    auto karnet = uslugiKarnetu.pobierzKarnetPoId(idKarnetu);
    if (!karnet) {
        std::cout << "Nie znaleziono karnetu o ID: " << idKarnetu << std::endl;
        return;
    }

    std::cout << "Czy na pewno chcesz usunąć karnet ID: " << idKarnetu
        << " (typ: " << karnet->pobierzTyp() << ")? (T/N): ";
    char potwierdzenie;
    std::cin >> potwierdzenie;

    if (potwierdzenie == 'T' || potwierdzenie == 't') {
        bool sukces = uslugiKarnetu.usunKarnet(idKarnetu);

        if (sukces) {
            std::cout << "Karnet został usunięty." << std::endl;
        }
        else {
            std::cout << "Wystąpił błąd podczas usuwania karnetu." << std::endl;
        }
    }
    else {
        std::cout << "Anulowano usuwanie karnetu." << std::endl;
    }
}

// === FUNKCJE POMOCNICZE ===

void funkcjaWPrzygotowaniu(const std::string& nazwaFunkcji) {
    std::cout << "\n=== " << nazwaFunkcji << " ===\n";
    std::cout << "Ta funkcja jest w przygotowaniu.\n";
    std::cout << "Będzie dostępna w następnej wersji systemu.\n\n";
    std::cout << "Obecne możliwości:\n";
    std::cout << "- Pełne zarządzanie klientami (dodaj, wyświetl, wyszukaj, usuń)\n";
    std::cout << "- Pełne zarządzanie karnetami (dodaj, wyświetl, usuń)\n";
    std::cout << "- Import/Export danych (CSV, JSON)\n\n";
    std::cout << "W przygotowaniu:\n";
    std::cout << "- Zarządzanie zajęciami grupowymi\n";
    std::cout << "- System rezerwacji\n";
    std::cout << "- Generowanie raportów\n";
    std::cout << "- Powiadomienia o wygasających karnetach\n";
}

// === FUNKCJA MAIN ===

int main() {
    // Ustawienie kodowania dla polskich znaków
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "=== Inicjalizacja systemu zarządzania siłownią ===\n";

    try {
        // Inicjalizacja systemu zarządzania bazą danych
        std::cout << "Inicjalizacja bazy danych...\n";
        MenedzerBD menedzerBD("silownia.db");

        // Inicjalizacja DAO
        std::cout << "Inicjalizacja warstwy dostępu do danych...\n";
        KlientDAO klientDAO(menedzerBD);
        KarnetDAO karnetDAO(menedzerBD);

        // Inicjalizacja serwisów
        std::cout << "Inicjalizacja serwisów biznesowych...\n";
        UslugiKlienta uslugiKlienta(klientDAO);
        UslugiKarnetu uslugiKarnetu(karnetDAO);

        std::cout << "System został pomyślnie zainicjalizowany!\n";

        int wybor;

        do {
            wyswietlMenu();
            std::cin >> wybor;

            // Sprawdzanie poprawności wejścia
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Nieprawidłowy wybór. Spróbuj ponownie.\n";
                continue;
            }

            switch (wybor) {
            case 1:
                dodajNowegoKlienta(uslugiKlienta);
                break;
            case 2:
                wyswietlWszystkichKlientow(uslugiKlienta);
                break;
            case 3:
                wyszukajKlientow(uslugiKlienta);
                break;
            case 4:
                usunKlienta(uslugiKlienta);
                break;
            case 5:
                dodajKarnetDlaKlienta(uslugiKlienta, uslugiKarnetu);
                break;
            case 6:
                wyswietlKarnetyKlienta(uslugiKlienta, uslugiKarnetu);
                break;
            case 7:
                usunKarnet(uslugiKarnetu);
                break;
            case 8:
                funkcjaWPrzygotowaniu("Zarządzanie zajęciami");
                break;
            case 9:
                funkcjaWPrzygotowaniu("Zarządzanie rezerwacjami");
                break;
            case 10:
                funkcjaWPrzygotowaniu("Generowanie raportów");
                break;
            case 11:
                funkcjaWPrzygotowaniu("Import/Export danych");
                break;
            case 0:
                std::cout << "\n=== Zamykanie systemu ===\n";
                std::cout << "Dziękujemy za korzystanie z systemu zarządzania siłownią!\n";
                std::cout << "Do widzenia!\n";
                break;
            default:
                std::cout << "Nieprawidłowy wybór. Wybierz opcję od 0 do 11.\n";
                break;
            }

            if (wybor != 0) {
                std::cout << "\nNaciśnij Enter, aby kontynuować...";
                std::cin.ignore();
                std::cin.get();
            }

        } while (wybor != 0);

    }
    catch (const std::exception& e) {
        std::cerr << "\n=== BŁĄD KRYTYCZNY ===\n";
        std::cerr << "Wystąpił błąd podczas działania systemu: " << e.what() << std::endl;
        std::cerr << "\nSystem zostanie zamknięty. Skontaktuj się z administratorem.\n";

        std::cout << "\nNaciśnij Enter, aby zamknąć...";
        std::cin.ignore();
        std::cin.get();

        return 1;
    }

    return 0;
}