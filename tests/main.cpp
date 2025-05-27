#define NOMINMAX
// tests/main.cpp
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <windows.h>
#include <limits>
#include <iomanip>
#include <fstream>
#include <vector>

// Poprawne include'y zgodne z polską strukturą projektu
#include "../database/menedzer_bd.h"
#include "../database/dao/klient_dao.h" 
#include "../database/dao/karnet_dao.h"
#include "../database/dao/zajecia_dao.h"
#include "../services/uslugi_klienta.h"
#include "../services/uslugi_karnetu.h"
#include "../services/uslugi_zajec.h"
#include "../services/uslugi_raportow.h"
#include "../models/klient.h"
#include "../models/karnet.h"
#include "../models/zajecia.h"
#include "../models/rezerwacja.h"
#include "../utils/eksport_danych.h"
#include "../utils/import_danych.h"

// === WSZYSTKIE DEKLARACJE FUNKCJI ===

// Funkcje do zarządzania klientami
void dodajNowegoKlienta(UslugiKlienta& uslugiKlienta);
void wyswietlWszystkichKlientow(UslugiKlienta& uslugiKlienta);
void wyszukajKlientow(UslugiKlienta& uslugiKlienta);
void usunKlienta(UslugiKlienta& uslugiKlienta);

// Funkcje do zarządzania karnetami
void dodajKarnetDlaKlienta(UslugiKlienta& uslugiKlienta, UslugiKarnetu& uslugiKarnetu);
void wyswietlKarnetyKlienta(UslugiKlienta& uslugiKlienta, UslugiKarnetu& uslugiKarnetu);
void usunKarnet(UslugiKarnetu& uslugiKarnetu);

// Funkcje do zarządzania zajęciami
void dodajZajecia(UslugiZajec& uslugiZajec);
void wyswietlWszystkieZajecia(UslugiZajec& uslugiZajec);
void zarzadzajRezerwacjami(UslugiZajec& uslugiZajec, UslugiKlienta& uslugiKlienta);

// Funkcje raportów
void generujRaportAktywnosci(UslugiRaportow& uslugiRaportow);
void generujRaportPopularnosci(UslugiRaportow& uslugiRaportow);
void generujRaportFinansowy(UslugiRaportow& uslugiRaportow);

// Funkcje eksportu
void eksportujDane(EksportDanych& eksportDanych);

// Funkcje pomocnicze
void funkcjaWPrzygotowaniu(const std::string& nazwaFunkcji);

// Funkcja wyświetlająca menu główne
void wyswietlMenu();

// === IMPLEMENTACJE FUNKCJI ===

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
    std::cout << "8. Dodaj zajęcia\n";
    std::cout << "9. Wyświetl wszystkie zajęcia\n";
    std::cout << "10. Zarządzanie rezerwacjami\n";
    std::cout << "\n=== RAPORTY ===\n";
    std::cout << "11. Raport aktywności klientów\n";
    std::cout << "12. Raport popularności zajęć\n";
    std::cout << "13. Raport finansowy\n";
    std::cout << "\n=== IMPORT/EXPORT ===\n";
    std::cout << "14. Eksport danych do CSV\n";
    std::cout << "15. Import danych z CSV\n";
    std::cout << "\n0. Wyjście\n";
    std::cout << "Wybierz opcję: ";
}

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

        switch (typKarnetu) {
        case 1:
            karnet = uslugiKarnetu.utworzKarnetMiesieczny(idKlienta, czyStudent);
            break;
        case 2:
            karnet = uslugiKarnetu.utworzKarnetKwartalny(idKlienta, czyStudent);
            break;
        case 3:
            karnet = uslugiKarnetu.utworzKarnetRoczny(idKlienta, czyStudent);
            break;
        default:
            std::cout << "Nieprawidłowy wybór rodzaju karnetu." << std::endl;
            return;
        }

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

void dodajZajecia(UslugiZajec& uslugiZajec) {
    std::cout << "\n=== Dodawanie nowych zajęć ===\n";

    std::string nazwa, trener, data, czas, opis;
    int maksUczestnikow, czasTrwania;

    std::cin.ignore(1000, '\n');

    std::cout << "Podaj nazwę zajęć: ";
    std::getline(std::cin, nazwa);

    std::cout << "Podaj trenera: ";
    std::getline(std::cin, trener);

    std::cout << "Podaj maksymalną liczbę uczestników: ";
    std::cin >> maksUczestnikow;

    std::cin.ignore();
    std::cout << "Podaj datę (RRRR-MM-DD): ";
    std::getline(std::cin, data);

    std::cout << "Podaj godzinę (HH:MM): ";
    std::getline(std::cin, czas);

    std::cout << "Podaj czas trwania (w minutach): ";
    std::cin >> czasTrwania;

    std::cin.ignore();
    std::cout << "Podaj opis: ";
    std::getline(std::cin, opis);

    Zajecia zajecia(-1, nazwa, trener, maksUczestnikow, data, czas, czasTrwania, opis);

    try {
        int id = uslugiZajec.dodajZajecia(zajecia);
        std::cout << "Dodano nowe zajęcia z ID: " << id << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd dodawania zajęć: " << e.what() << std::endl;
    }
}

void wyswietlWszystkieZajecia(UslugiZajec& uslugiZajec) {
    auto zajecia = uslugiZajec.pobierzWszystkieZajecia();

    if (zajecia.empty()) {
        std::cout << "Brak zajęć w bazie danych." << std::endl;
        return;
    }

    std::cout << "\n=== Lista zajęć ===\n";
    std::cout << std::left
        << std::setw(5) << "ID"
        << std::setw(20) << "Nazwa"
        << std::setw(15) << "Trener"
        << std::setw(12) << "Data"
        << std::setw(8) << "Czas"
        << std::setw(10) << "Miejsca"
        << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    for (const auto& zajecie : zajecia) {
        std::cout << std::left
            << std::setw(5) << zajecie.pobierzId()
            << std::setw(20) << zajecie.pobierzNazwe()
            << std::setw(15) << zajecie.pobierzTrenera()
            << std::setw(12) << zajecie.pobierzDate()
            << std::setw(8) << zajecie.pobierzCzas()
            << std::setw(10) << zajecie.pobierzMaksUczestnikow()
            << std::endl;
    }
}

void zarzadzajRezerwacjami(UslugiZajec& uslugiZajec, UslugiKlienta& uslugiKlienta) {
    std::cout << "\n=== Zarządzanie rezerwacjami ===\n";
    std::cout << "1. Dodaj rezerwację\n";
    std::cout << "2. Wyświetl rezerwacje klienta\n";
    std::cout << "3. Anuluj rezerwację\n";
    std::cout << "Wybierz opcję: ";

    int wybor;
    std::cin >> wybor;

    switch (wybor) {
    case 1: {
        int idKlienta, idZajec;

        std::cout << "Podaj ID klienta: ";
        std::cin >> idKlienta;

        auto klient = uslugiKlienta.pobierzKlientaPoId(idKlienta);
        if (!klient) {
            std::cout << "Nie znaleziono klienta o ID: " << idKlienta << std::endl;
            return;
        }

        std::cout << "Podaj ID zajęć: ";
        std::cin >> idZajec;

        auto zajecia = uslugiZajec.pobierzZajeciaPoId(idZajec);
        if (!zajecia) {
            std::cout << "Nie znaleziono zajęć o ID: " << idZajec << std::endl;
            return;
        }

        // Sprawdź dostępne miejsca
        int dostepneMiejsca = uslugiZajec.pobierzDostepneMiejscaZajec(idZajec);
        if (dostepneMiejsca <= 0) {
            std::cout << "Brak wolnych miejsc na tych zajęciach." << std::endl;
            return;
        }

        // Sprawdź czy klient ma aktywny karnet
        if (!uslugiZajec.czyKlientUprawniony(idKlienta, idZajec)) {
            std::cout << "Klient nie ma aktywnego karnetu." << std::endl;
            return;
        }

        Rezerwacja rezerwacja(-1, idKlienta, idZajec, "", "potwierdzona");

        try {
            int id = uslugiZajec.dodajRezerwacje(rezerwacja);
            std::cout << "Dodano rezerwację z ID: " << id << std::endl;
            std::cout << "Pozostałe miejsca: " << (dostepneMiejsca - 1) << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Błąd dodawania rezerwacji: " << e.what() << std::endl;
        }
        break;
    }
    case 2: {
        int idKlienta;
        std::cout << "Podaj ID klienta: ";
        std::cin >> idKlienta;

        auto rezerwacje = uslugiZajec.pobierzRezerwacjeKlienta(idKlienta);

        if (rezerwacje.empty()) {
            std::cout << "Klient nie ma żadnych rezerwacji." << std::endl;
            return;
        }

        std::cout << "\n=== Rezerwacje klienta ===\n";
        for (const auto& r : rezerwacje) {
            std::cout << "ID: " << r.pobierzId()
                << ", Zajęcia ID: " << r.pobierzIdZajec()
                << ", Data: " << r.pobierzDateRezerwacji()
                << ", Status: " << r.pobierzStatus() << std::endl;
        }
        break;
    }
    case 3: {
        int idRezerwacji;
        std::cout << "Podaj ID rezerwacji do anulowania: ";
        std::cin >> idRezerwacji;

        bool sukces = uslugiZajec.anulujRezerwacje(idRezerwacji);
        if (sukces) {
            std::cout << "Rezerwacja została anulowana." << std::endl;
        }
        else {
            std::cout << "Błąd anulowania rezerwacji." << std::endl;
        }
        break;
    }
    default:
        std::cout << "Nieprawidłowy wybór." << std::endl;
    }
}

void generujRaportAktywnosci(UslugiRaportow& uslugiRaportow) {
    std::string dataOd, dataDo;

    std::cin.ignore();
    std::cout << "Podaj datę początkową (RRRR-MM-DD): ";
    std::getline(std::cin, dataOd);

    std::cout << "Podaj datę końcową (RRRR-MM-DD): ";
    std::getline(std::cin, dataDo);

    try {
        auto raport = uslugiRaportow.generujRaportAktywnosciKlienta(dataOd, dataDo);

        std::cout << "\n=== Raport aktywności klientów ===\n";
        std::cout << "Okres: " << dataOd << " - " << dataDo << "\n\n";

        if (raport.empty()) {
            std::cout << "Brak danych w podanym okresie.\n";
            return;
        }

        for (const auto& r : raport) {
            std::cout << "Klient: " << r.nazwaKlienta << "\n";
            std::cout << "  Łączna liczba zajęć: " << r.lacznaLiczbaZajec << "\n";
            std::cout << "  Liczba anulowanych: " << r.liczbaAnulowanychZajec << "\n";
            std::cout << "  Najczęstsza aktywność: " << r.najczestszaZajecia << "\n";
            std::cout << "  Ostatnia wizyta: " << r.ostatniaWizyta << "\n\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd generowania raportu: " << e.what() << std::endl;
    }
}

void generujRaportPopularnosci(UslugiRaportow& uslugiRaportow) {
    std::string dataOd, dataDo;

    std::cin.ignore();
    std::cout << "Podaj datę początkową (RRRR-MM-DD): ";
    std::getline(std::cin, dataOd);

    std::cout << "Podaj datę końcową (RRRR-MM-DD): ";
    std::getline(std::cin, dataDo);

    try {
        auto raport = uslugiRaportow.generujRaportPopularnosciZajec(dataOd, dataDo);

        std::cout << "\n=== Raport popularności zajęć ===\n";
        std::cout << "Okres: " << dataOd << " - " << dataDo << "\n\n";

        if (raport.empty()) {
            std::cout << "Brak danych w podanym okresie.\n";
            return;
        }

        for (const auto& r : raport) {
            std::cout << "Zajęcia: " << r.nazwaZajec << "\n";
            std::cout << "  Trener: " << r.trener << "\n";
            std::cout << "  Liczba rezerwacji: " << r.lacznaLiczbaRezerwacji << "\n";
            std::cout << "  Stopień wypełnienia: " << std::fixed << std::setprecision(1)
                << r.stopienWypelnienia << "%\n\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd generowania raportu: " << e.what() << std::endl;
    }
}

void generujRaportFinansowy(UslugiRaportow& uslugiRaportow) {
    std::string dataOd, dataDo;

    std::cin.ignore();
    std::cout << "Podaj datę początkową (RRRR-MM-DD): ";
    std::getline(std::cin, dataOd);

    std::cout << "Podaj datę końcową (RRRR-MM-DD): ";
    std::getline(std::cin, dataDo);

    try {
        auto raport = uslugiRaportow.generujRaportFinansowy(dataOd, dataDo);

        std::cout << "\n=== Raport finansowy ===\n";
        std::cout << "Okres: " << dataOd << " - " << dataDo << "\n\n";

        std::cout << "Łączny przychód: " << std::fixed << std::setprecision(2)
            << raport.lacznyPrzychod << " zł\n";
        std::cout << "Liczba karnetów: " << raport.lacznaLiczbaKarnetow << "\n";
        std::cout << "Przychód studencki: " << raport.przychodStudencki << " zł\n";
        std::cout << "Przychód standardowy: " << raport.przychodStandardowy << " zł\n\n";

        std::cout << "Przychód wg typu karnetu:\n";
        for (const auto& para : raport.przychodWgTypuKarnetu) {
            std::cout << "  " << para.first << ": " << para.second << " zł\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd generowania raportu: " << e.what() << std::endl;
    }
}

void eksportujDane(EksportDanych& eksportDanych) {
    std::cout << "\n=== Eksport danych ===\n";
    std::cout << "1. Eksportuj klientów do CSV\n";
    std::cout << "2. Eksportuj karnety do CSV\n";
    std::cout << "3. Eksportuj zajęcia do CSV\n";
    std::cout << "Wybierz opcję: ";

    int wybor;
    std::cin >> wybor;

    try {
        switch (wybor) {
        case 1:
            eksportDanych.eksportujKlientowDoCSV("C:/Users/Kacper/OneDrive/Pulpit/SnakeAI/Silka/eksport/klienci_export.csv");
            std::cout << "Eksportowano klientów do pliku klienci_export.csv\n";
            break;
        case 2:
            eksportDanych.eksportujKarnetyDoCSV("C:/Users/Kacper/OneDrive/Pulpit/SnakeAI/Silka/eksport/karnety_export.csv");
            std::cout << "Eksportowano karnety do pliku karnety_export.csv\n";
            break;
        case 3:
            eksportDanych.eksportujZajeciaDoCSV("C:/Users/Kacper/OneDrive/Pulpit/SnakeAI/Silka/eksport/zajecia_export.csv");
            std::cout << "Eksportowano zajęcia do pliku zajecia_export.csv\n";
            break;
        default:
            std::cout << "Nieprawidłowy wybór.\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd eksportu: " << e.what() << std::endl;
    }
}

void importujDane(ImportDanych& importDanych) {
    std::cout << "\n=== Import danych z CSV ===\n";
    std::cout << "1. Importuj klientów z CSV\n";
    std::cout << "2. Importuj karnety z CSV\n";
    std::cout << "3. Importuj zajęcia z CSV\n";
    std::cout << "Wybierz opcję: ";

    int wybor;
    std::cin >> wybor;

    std::string sciezkaPliku;
    std::cin.ignore();

    try {
        switch (wybor) {
        case 1: {
            std::cout << "Podaj ścieżkę do pliku CSV z klientami: ";
            std::getline(std::cin, sciezkaPliku);

            if (sciezkaPliku.empty()) {
                sciezkaPliku = "klienci_import.csv";
                std::cout << "Używam domyślnej ścieżki: " << sciezkaPliku << std::endl;
            }

            std::cout << "Importowanie klientów z pliku: " << sciezkaPliku << std::endl;

            auto klienci = importDanych.importujKlientowZCSV(sciezkaPliku);

            if (klienci.empty()) {
                std::cout << "Nie znaleziono żadnych klientów do importu." << std::endl;
                return;
            }

            std::cout << "Znaleziono " << klienci.size() << " klientów do importu." << std::endl;
            std::cout << "\nPodgląd pierwszych 3 klientów:" << std::endl;

            for (size_t i = 0; i < std::min(static_cast<size_t>(3), klienci.size()); ++i) {
                const auto& klient = klienci[i];
                std::cout << "- " << klient.pobierzImie() << " " << klient.pobierzNazwisko()
                    << " (" << klient.pobierzEmail() << ")" << std::endl;
            }

            std::cout << "\nCzy chcesz kontynuować import? (T/N): ";
            char potwierdzenie;
            std::cin >> potwierdzenie;

            if (potwierdzenie == 'T' || potwierdzenie == 't') {
                std::cout << "Zapisywanie klientów do bazy danych..." << std::endl;
                importDanych.zapiszZaimportowanychKlientow(klienci);
                std::cout << "Pomyślnie zaimportowano " << klienci.size() << " klientów!" << std::endl;
            }
            else {
                std::cout << "Import został anulowany." << std::endl;
            }
            break;
        }
        case 2: {
            std::cout << "Podaj ścieżkę do pliku CSV z karnetami: ";
            std::getline(std::cin, sciezkaPliku);

            if (sciezkaPliku.empty()) {
                sciezkaPliku = "karnety_import.csv";
                std::cout << "Używam domyślnej ścieżki: " << sciezkaPliku << std::endl;
            }

            std::cout << "Importowanie karnetów z pliku: " << sciezkaPliku << std::endl;

            auto karnety = importDanych.importujKarnetyZCSV(sciezkaPliku);

            if (karnety.empty()) {
                std::cout << "Nie znaleziono żadnych karnetów do importu." << std::endl;
                return;
            }

            std::cout << "Znaleziono " << karnety.size() << " karnetów do importu." << std::endl;
            std::cout << "\nPodgląd pierwszych 3 karnetów:" << std::endl;

            for (size_t i = 0; i < std::min(static_cast<size_t>(3), karnety.size()); ++i) {
                const auto& karnet = karnety[i];
                std::cout << "- Typ: " << karnet.pobierzTyp()
                    << ", Klient ID: " << karnet.pobierzIdKlienta()
                    << ", Cena: " << karnet.pobierzCene() << " zł" << std::endl;
            }

            std::cout << "\nCzy chcesz kontynuować import? (T/N): ";
            char potwierdzenie;
            std::cin >> potwierdzenie;

            if (potwierdzenie == 'T' || potwierdzenie == 't') {
                std::cout << "Zapisywanie karnetów do bazy danych..." << std::endl;
                importDanych.zapiszZaimportowaneKarnety(karnety);
                std::cout << "Pomyślnie zaimportowano " << karnety.size() << " karnetów!" << std::endl;
            }
            else {
                std::cout << "Import został anulowany." << std::endl;
            }
            break;
        }
        case 3: {
            std::cout << "Podaj ścieżkę do pliku CSV z zajęciami: ";
            std::getline(std::cin, sciezkaPliku);

            if (sciezkaPliku.empty()) {
                sciezkaPliku = "zajecia_import.csv";
                std::cout << "Używam domyślnej ścieżki: " << sciezkaPliku << std::endl;
            }

            std::cout << "Importowanie zajęć z pliku: " << sciezkaPliku << std::endl;

            auto zajecia = importDanych.importujZajeciaZCSV(sciezkaPliku);

            if (zajecia.empty()) {
                std::cout << "Nie znaleziono żadnych zajęć do importu." << std::endl;
                return;
            }

            std::cout << "Znaleziono " << zajecia.size() << " zajęć do importu." << std::endl;
            std::cout << "\nPodgląd pierwszych 3 zajęć:" << std::endl;

            for (size_t i = 0; i < std::min(static_cast<size_t>(3), zajecia.size()); ++i) {
                const auto& zajecie = zajecia[i];
                std::cout << "- " << zajecie.pobierzNazwe()
                    << ", Trener: " << zajecie.pobierzTrenera()
                    << ", Data: " << zajecie.pobierzDate()
                    << " " << zajecie.pobierzCzas() << std::endl;
            }

            std::cout << "\nCzy chcesz kontynuować import? (T/N): ";
            char potwierdzenie;
            std::cin >> potwierdzenie;

            if (potwierdzenie == 'T' || potwierdzenie == 't') {
                std::cout << "Zapisywanie zajęć do bazy danych..." << std::endl;
                importDanych.zapiszZaimportowaneZajecia(zajecia);
                std::cout << "Pomyślnie zaimportowano " << zajecia.size() << " zajęć!" << std::endl;
            }
            else {
                std::cout << "Import został anulowany." << std::endl;
            }
            break;
        }
        default:
            std::cout << "Nieprawidłowy wybór." << std::endl;
            return;
        }

        std::cout << "\n=== Informacje o formatach CSV ===\n";
        std::cout << "Klienci: id,imie,nazwisko,email,telefon,data_urodzenia,data_rejestracji,uwagi\n";
        std::cout << "Karnety: id,id_klienta,typ,data_rozpoczecia,data_zakonczenia,cena,czy_aktywny\n";
        std::cout << "Zajęcia: id,nazwa,trener,maks_uczestnikow,data,czas,czas_trwania,opis\n";

    }
    catch (const WyjatekImportu& e) {
        std::cerr << "Błąd importu: " << e.what() << std::endl;
        std::cout << "\nSprawdź czy:\n";
        std::cout << "- Plik istnieje i ma poprawną ścieżkę\n";
        std::cout << "- Plik ma prawidłowy format CSV\n";
        std::cout << "- Pierwsza linia zawiera nagłówki kolumn\n";
        std::cout << "- Dane w kolumnach są zgodne z oczekiwanym formatem\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Nieoczekiwany błąd: " << e.what() << std::endl;
    }
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
        ZajeciaDAO zajeciaDAO(menedzerBD);

        // Inicjalizacja serwisów
        std::cout << "Inicjalizacja serwisów biznesowych...\n";
        UslugiKlienta uslugiKlienta(klientDAO);
        UslugiKarnetu uslugiKarnetu(karnetDAO);
        UslugiZajec uslugiZajec(zajeciaDAO, uslugiKarnetu);
        UslugiRaportow uslugiRaportow(menedzerBD);

        // Inicjalizacja narzędzi import/export
        ImportDanych importDanych(uslugiKlienta, uslugiKarnetu, uslugiZajec);
        EksportDanych eksportDanych(uslugiKlienta, uslugiKarnetu, uslugiZajec);

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
                dodajZajecia(uslugiZajec);
                break;
            case 9:
                wyswietlWszystkieZajecia(uslugiZajec);
                break;
            case 10:
                zarzadzajRezerwacjami(uslugiZajec, uslugiKlienta);
                break;
            case 11:
                generujRaportAktywnosci(uslugiRaportow);
                break;
            case 12:
                generujRaportPopularnosci(uslugiRaportow);
                break;
            case 13:
                generujRaportFinansowy(uslugiRaportow);
                break;
            case 14:
                eksportujDane(eksportDanych);
                break;
            case 15:
                importujDane(importDanych);
                break;
            case 0:
                std::cout << "\n=== Zamykanie systemu ===\n";
                std::cout << "Dziękujemy za korzystanie z systemu zarządzania siłownią!\n";
                std::cout << "Do widzenia!\n";
                break;
            default:
                std::cout << "Nieprawidłowy wybór. Wybierz opcję od 0 do 15.\n";
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