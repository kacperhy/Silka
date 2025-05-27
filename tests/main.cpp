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
#include "../services/uslugi_raportow.h"
#include "../utils/historia_zmian.h"
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
    std::cout << "\n=== RAPORTY ===\n";
    std::cout << "8. Raport aktywności klienta\n";
    std::cout << "9. Raport finansowy\n";
    std::cout << "10. Raport wszystkich klientów\n";
    std::cout << "11. Raport karnetów\n";
    std::cout << "\n=== HISTORIA I COFANIE ===\n";
    std::cout << "12. Pokaż historię zmian\n";
    std::cout << "13. Cofnij ostatnią operację\n";
    std::cout << "14. Punkty przywracania\n";
    std::cout << "15. Raport historii\n";
    std::cout << "\n=== INNE ===\n";
    std::cout << "16. Zajęcia i rezerwacje (w przygotowaniu)\n";
    std::cout << "17. Import/Export danych (w przygotowaniu)\n";
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

// === FUNKCJE DO RAPORTÓW ===

void menuRaportow(UslugiRaportow& uslugiRaportow) {
    int wybor;

    std::cout << "\n=== MENU RAPORTÓW ===\n";
    std::cout << "1. Raport aktywności klienta\n";
    std::cout << "2. Raport finansowy\n";
    std::cout << "3. Raport wszystkich klientów\n";
    std::cout << "4. Raport karnetów\n";
    std::cout << "0. Powrót\n";
    std::cout << "Wybierz opcję: ";
    std::cin >> wybor;

    std::string raport;
    std::string nazwaPliku;

    try {
        switch (wybor) {
        case 1: {
            int idKlienta;
            std::string dataOd, dataDo;

            std::cout << "Podaj ID klienta: ";
            std::cin >> idKlienta;

            std::cin.ignore();
            std::cout << "Podaj datę od (RRRR-MM-DD) lub Enter dla domyślnej: ";
            std::getline(std::cin, dataOd);
            if (dataOd.empty()) dataOd = "2024-01-01";

            std::cout << "Podaj datę do (RRRR-MM-DD) lub Enter dla domyślnej: ";
            std::getline(std::cin, dataDo);
            if (dataDo.empty()) dataDo = "2025-12-31";

            raport = uslugiRaportow.generujRaportAktywnosci(idKlienta, dataOd, dataDo);
            nazwaPliku = "raport_aktywnosci_klient_" + std::to_string(idKlienta);
            break;
        }
        case 2: {
            std::string miesiac;
            std::cin.ignore();
            std::cout << "Podaj miesiąc (RRRR-MM) lub Enter dla bieżącego: ";
            std::getline(std::cin, miesiac);
            if (miesiac.empty()) miesiac = "2025-05";

            raport = uslugiRaportow.generujRaportFinansowy(miesiac);
            nazwaPliku = "raport_finansowy_" + miesiac;
            break;
        }
        case 3: {
            raport = uslugiRaportow.generujRaportKlientow();
            nazwaPliku = "raport_klientow";
            break;
        }
        case 4: {
            raport = uslugiRaportow.generujRaportKarnetow();
            nazwaPliku = "raport_karnetow";
            break;
        }
        case 0:
            return;
        default:
            std::cout << "Nieprawidłowy wybór.\n";
            return;
        }

        // Wyświetl raport
        std::cout << "\n" << raport << std::endl;

        // Zapytaj o zapis do pliku
        char zapisac;
        std::cout << "Czy zapisać raport do pliku? (T/N): ";
        std::cin >> zapisac;

        if (zapisac == 'T' || zapisac == 't') {
            std::cout << "\nWybierz format:\n";
            std::cout << "1. TXT\n";
            std::cout << "2. HTML\n";
            std::cout << "Wybierz: ";

            int formatWybor;
            std::cin >> formatWybor;

            std::string format = (formatWybor == 2) ? "HTML" : "TXT";
            std::string rozszerzenie = (formatWybor == 2) ? ".html" : ".txt";
            std::string sciezka = nazwaPliku + rozszerzenie;

            if (uslugiRaportow.zapisRaportDoPliku(raport, format, sciezka)) {
                std::cout << "Raport zapisano do pliku: " << sciezka << std::endl;
            }
            else {
                std::cout << "Błąd zapisu raportu do pliku." << std::endl;
            }
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Błąd generowania raportu: " << e.what() << std::endl;
    }
}

// === FUNKCJE DO HISTORII ZMIAN ===

void pokazHistorieZmian(HistoriaZmian& historia) {
    std::cout << "\n=== HISTORIA ZMIAN ===\n";

    int limit;
    std::cout << "Ile ostatnich operacji pokazać (0 = wszystkie): ";
    std::cin >> limit;

    auto logi = historia.pobierzHistorie(limit > 0 ? limit : 0);

    if (logi.empty()) {
        std::cout << "\nBrak operacji w historii.\n";
        return;
    }

    std::cout << "\nOSTATNIE OPERACJE:\n";
    std::cout << std::string(80, '=') << "\n";

    for (const auto& log : logi) {
        std::cout << "ID: " << log.id
            << " | " << log.typOperacji
            << " | " << log.tabela;

        if (log.idRekordu > 0) {
            std::cout << " | Rekord: " << log.idRekordu;
        }

        std::cout << "\n    Czas: " << log.czasOperacji;

        if (!log.opis.empty()) {
            std::cout << "\n    Opis: " << log.opis;
        }

        std::cout << "\n" << std::string(80, '-') << "\n";
    }
}

void cofnijOperacje(HistoriaZmian& historia) {
    std::cout << "\n=== COFANIE OPERACJI ===\n";

    char wybor;
    std::cout << "1. Cofnij ostatnią operację\n";
    std::cout << "2. Cofnij konkretną operację (podaj ID)\n";
    std::cout << "Wybierz opcję (1/2): ";
    std::cin >> wybor;

    if (wybor == '1') {
        if (historia.cofnijOstatnia()) {
            std::cout << "Operacja została cofnięta.\n";
        }
        else {
            std::cout << "Nie udało się cofnąć operacji.\n";
        }
    }
    else if (wybor == '2') {
        int idOperacji;
        std::cout << "Podaj ID operacji do cofnięcia: ";
        std::cin >> idOperacji;

        if (historia.cofnijOperacje(idOperacji)) {
            std::cout << "Operacja została cofnięta.\n";
        }
        else {
            std::cout << "Nie udało się cofnąć operacji.\n";
        }
    }
    else {
        std::cout << "Nieprawidłowy wybór.\n";
    }
}

void menuPunktowPrzywracania(HistoriaZmian& historia) {
    int wybor;

    std::cout << "\n=== PUNKTY PRZYWRACANIA ===\n";
    std::cout << "1. Pokaż punkty przywracania\n";
    std::cout << "2. Utwórz nowy punkt przywracania\n";
    std::cout << "3. Przywróć do punktu\n";
    std::cout << "4. Usuń punkt przywracania\n";
    std::cout << "0. Powrót\n";
    std::cout << "Wybierz opcję: ";
    std::cin >> wybor;

    try {
        switch (wybor) {
        case 1: {
            auto punkty = historia.pobierzPunktyPrzywracania();

            if (punkty.empty()) {
                std::cout << "\nBrak punktów przywracania.\n";
            }
            else {
                std::cout << "\nPUNKTY PRZYWRACANIA:\n";
                std::cout << std::string(60, '=') << "\n";

                for (const auto& punkt : punkty) {
                    std::cout << "ID: " << punkt.id
                        << " | \"" << punkt.nazwa << "\"\n";
                    std::cout << "Utworzony: " << punkt.czasUtworzenia << "\n";

                    if (!punkt.opis.empty()) {
                        std::cout << "Opis: " << punkt.opis << "\n";
                    }

                    std::cout << std::string(60, '-') << "\n";
                }
            }
            break;
        }
        case 2: {
            std::string nazwa, opis;

            std::cin.ignore();
            std::cout << "Podaj nazwę punktu przywracania: ";
            std::getline(std::cin, nazwa);

            std::cout << "Podaj opis (opcjonalny): ";
            std::getline(std::cin, opis);

            int id = historia.utworzPunktPrzywracania(nazwa, opis);
            if (id > 0) {
                std::cout << "Utworzono punkt przywracania z ID: " << id << std::endl;
            }
            else {
                std::cout << "Błąd tworzenia punktu przywracania.\n";
            }
            break;
        }
        case 3: {
            int idPunktu;
            std::cout << "Podaj ID punktu przywracania: ";
            std::cin >> idPunktu;

            std::cout << "UWAGA: Ta operacja cofnie wszystkie zmiany wykonane po utworzeniu punktu!\n";
            std::cout << "Czy na pewno kontynuować? (T/N): ";

            char potwierdzenie;
            std::cin >> potwierdzenie;

            if (potwierdzenie == 'T' || potwierdzenie == 't') {
                if (historia.przywrocDoPunktu(idPunktu)) {
                    std::cout << "Pomyślnie przywrócono do punktu.\n";
                }
                else {
                    std::cout << "Błąd przywracania do punktu.\n";
                }
            }
            else {
                std::cout << "Anulowano operację.\n";
            }
            break;
        }
        case 4: {
            int idPunktu;
            std::cout << "Podaj ID punktu do usunięcia: ";
            std::cin >> idPunktu;

            if (historia.usunPunktPrzywracania(idPunktu)) {
                std::cout << "Punkt przywracania został usunięty.\n";
            }
            else {
                std::cout << "Błąd usuwania punktu przywracania.\n";
            }
            break;
        }
        case 0:
            return;
        default:
            std::cout << "Nieprawidłowy wybór.\n";
            break;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
    }
}

void pokazRaportHistorii(HistoriaZmian& historia) {
    std::cout << "\n=== GENEROWANIE RAPORTU HISTORII ===\n";

    try {
        std::string raport = historia.generujRaportHistorii();
        std::cout << raport << std::endl;

        char zapisac;
        std::cout << "Czy zapisać raport do pliku? (T/N): ";
        std::cin >> zapisac;

        if (zapisac == 'T' || zapisac == 't') {
            std::string sciezka = "raport_historii.txt";
            std::ofstream plik(sciezka);

            if (plik.is_open()) {
                plik << raport;
                plik.close();
                std::cout << "Raport zapisano do pliku: " << sciezka << std::endl;
            }
            else {
                std::cout << "Błąd zapisu raportu do pliku.\n";
            }
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Błąd generowania raportu historii: " << e.what() << std::endl;
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
    // Ustawienia Windows - polskie znaki i kodowanie
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    system("chcp 65001 >nul"); // Ustawienie UTF-8

    // Ustawienie tytułu okna konsoli
    SetConsoleTitleA("System Zarządzania Siłownią v1.0");

    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║          SYSTEM ZARZĄDZANIA SIŁOWNIĄ - WINDOWS               ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "🏋️‍♂️ Witamy w profesjonalnym systemie zarządzania siłownią!\n\n";

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
        UslugiRaportow uslugiRaportow(uslugiKlienta, uslugiKarnetu);
        HistoriaZmian historia(menedzerBD);

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
            case 9:
            case 10:
            case 11:
                menuRaportow(uslugiRaportow);
                break;
            case 12:
                pokazHistorieZmian(historia);
                break;
            case 13:
                cofnijOperacje(historia);
                break;
            case 14:
                menuPunktowPrzywracania(historia);
                break;
            case 15:
                pokazRaportHistorii(historia);
                break;
            case 16:
                funkcjaWPrzygotowaniu("Zarządzanie zajęciami i rezerwacjami");
                break;
            case 17:
                funkcjaWPrzygotowaniu("Import/Export danych");
                break;
            case 0:
                std::cout << "\n=== Zamykanie systemu ===\n";
                std::cout << "Dziękujemy za korzystanie z systemu zarządzania siłownią!\n";
                std::cout << "Do widzenia!\n";
                break;
            default:
                std::cout << "Nieprawidłowy wybór. Wybierz opcję od 0 do 17.\n";
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