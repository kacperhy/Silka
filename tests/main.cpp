#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <windows.h>
#include <limits>
#include <iomanip>

#include "../database/menedzer_bd.h"
#include "../database/dao/klient_dao.h"
#include "../database/dao/karnet_dao.h"
#include "../database/dao/zajecia_dao.h"
#include "../services/uslugi_klienta.h"
#include "../services/uslugi_karnetu.h"
#include "../services/uslugi_zajec.h"
#include "../services/uslugi_raportow.h"
#include "../utils/import_danych.h"
#include "../utils/eksport_danych.h"
#include "../models/klient.h"
#include "../models/karnet.h"
#include "../models/zajecia.h"
#include "../models/rezerwacja.h"

void usunKlientaPoId(UslugiKlienta& uslugiKlienta) {
    int idKlienta;

    std::cout << "Podaj ID klienta do usunięcia: ";
    std::cin >> idKlienta;

    auto klient = uslugiKlienta.pobierzKlientaPoId(idKlienta);
    if (!klient) {
        std::cout << "Nie znaleziono klienta o ID: " << idKlienta << std::endl;
        return;
    }

    std::cout << "Czy na pewno chcesz usunąć klienta " << klient->pobierzPelneNazwisko() << " (ID: " << idKlienta << ")? (T/N): ";
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
            << std::setw(30) << klient.pobierzImie() + " " + klient.pobierzNazwisko()
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
    std::cout << "ID\tImię i nazwisko\tEmail\tTelefon\tData rejestracji\n";

    for (const auto& klient : klienci) {
        std::cout << klient.pobierzId() << "\t"
            << klient.pobierzImie() << " " << klient.pobierzNazwisko() << "\t"
            << klient.pobierzEmail() << "\t"
            << klient.pobierzTelefon() << "\t"
            << klient.pobierzDateRejestracji() << "\n";
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

    Karnet karnet;
    std::string nazwaTypuKarnetu;

    try {
        switch (typKarnetu) {
        case 1:
            karnet = uslugiKarnetu.utworzKarnetMiesieczny(idKlienta, czyStudent);
            nazwaTypuKarnetu = czyStudent ? "student_miesieczny" : "normalny_miesieczny";
            break;
        case 2:
            karnet = uslugiKarnetu.utworzKarnetKwartalny(idKlienta, czyStudent);
            nazwaTypuKarnetu = czyStudent ? "student_kwartalny" : "normalny_kwartalny";
            break;
        case 3:
            karnet = uslugiKarnetu.utworzKarnetRoczny(idKlienta, czyStudent);
            nazwaTypuKarnetu = czyStudent ? "student_roczny" : "normalny_roczny";
            break;
        default:
            std::cout << "Nieprawidłowy wybór rodzaju karnetu." << std::endl;
            return;
        }

        karnet.ustawTyp(nazwaTypuKarnetu);

        std::cout << "DIAGNOSTYKA - Dane karnetu przed dodaniem:\n";
        std::cout << "  ID Klienta: " << karnet.pobierzIdKlienta() << "\n";
        std::cout << "  Typ: [" << karnet.pobierzTyp() << "]\n";
        std::cout << "  Data rozpoczęcia: [" << karnet.pobierzDateRozpoczecia() << "]\n";
        std::cout << "  Data zakończenia: [" << karnet.pobierzDateZakonczenia() << "]\n";
        std::cout << "  Cena: [" << karnet.pobierzCene() << "]\n";

        int id = uslugiKarnetu.dodajKarnet(karnet);
        std::cout << "Dodano nowy karnet z ID: " << id << std::endl;
        std::cout << "Termin ważności: od " << karnet.pobierzDateRozpoczecia() << " do " << karnet.pobierzDateZakonczenia() << std::endl;
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
        std::cout << "Klient " << klient->pobierzPelneNazwisko() << " nie posiada żadnych karnetów." << std::endl;
        return;
    }

    std::cout << "\n=== Karnety klienta " << klient->pobierzPelneNazwisko() << " ===\n";
    std::cout << std::left
        << std::setw(8) << "ID"
        << std::setw(15) << "Rodzaj"
        << std::setw(25) << "Data rozpoczęcia"
        << std::setw(25) << "Data zakończenia"
        << std::setw(10) << "Cena"
        << std::setw(8) << "Aktywny"
        << std::setw(8) << "Ważny"
        << std::setw(15) << "Dni pozostałe"
        << std::endl;
    std::cout << std::string(110, '-') << std::endl;

    std::cout << "DIAGNOSTYKA - Liczba znalezionych karnetów: " << karnety.size() << std::endl;

    for (const auto& karnet : karnety) {
        std::cout << "DIAGNOSTYKA - Dane karnetu ID " << karnet.pobierzId() << ":\n";
        std::cout << "  Typ: [" << karnet.pobierzTyp() << "]\n";
        std::cout << "  Data rozpoczęcia: [" << karnet.pobierzDateRozpoczecia() << "]\n";
        std::cout << "  Data zakończenia: [" << karnet.pobierzDateZakonczenia() << "]\n";
        std::cout << "  Cena: [" << karnet.pobierzCene() << "]\n";
        std::cout << "  Czy aktywny: [" << (karnet.pobierzCzyAktywny() ? "Tak" : "Nie") << "]\n";

        std::cout << std::left
            << std::setw(8) << karnet.pobierzId()
            << std::setw(15) << karnet.pobierzTyp()
            << std::setw(25) << karnet.pobierzDateRozpoczecia()
            << std::setw(25) << karnet.pobierzDateZakonczenia()
            << std::setw(10) << karnet.pobierzCene() << " zł"
            << std::setw(8) << (karnet.pobierzCzyAktywny() ? "Tak" : "Nie")
            << std::setw(8) << (karnet.czyWazny() ? "Tak" : "Nie")
            << std::setw(15) << karnet.ileDniPozostalo()
            << std::endl;
    }
}

void dodajZajeciaGrupowe(UslugiZajec& uslugiZajec) {
    std::string nazwa, trener, data, czas, opis;
    int maksUczestnikow, czasTrwania;

    std::cin.ignore();
    std::cout << "Podaj nazwę zajęć: ";
    std::getline(std::cin, nazwa);

    std::cout << "Podaj nazwisko trenera: ";
    std::getline(std::cin, trener);

    std::cout << "Podaj maksymalną liczbę uczestników: ";
    std::cin >> maksUczestnikow;

    std::cin.ignore();
    std::cout << "Podaj datę zajęć (RRRR-MM-DD): ";
    std::getline(std::cin, data);

    std::cout << "Podaj godzinę zajęć (GG:MM): ";
    std::getline(std::cin, czas);

    std::cout << "Podaj czas trwania zajęć (w minutach): ";
    std::cin >> czasTrwania;

    std::cin.ignore();
    std::cout << "Podaj opis zajęć: ";
    std::getline(std::cin, opis);

    Zajecia zajecia(-1, nazwa, trener, maksUczestnikow, data, czas, czasTrwania, opis);

    try {
        int id = uslugiZajec.dodajZajecia(zajecia);
        std::cout << "Dodano nowe zajęcia grupowe z ID: " << id << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
    }
}

void wyswietlWszystkieZajecia(UslugiZajec& uslugiZajec) {
    auto zajecia = uslugiZajec.pobierzWszystkieZajecia();

    if (zajecia.empty()) {
        std::cout << "Brak zajęć w bazie danych." << std::endl;
        return;
    }

    std::cout << "\n=== Lista zajęć ===\n";
    std::cout << "ID\tNazwa\tTrener\tData\tGodzina\tCzas trwania\tMiejsca\tDostępne miejsca\n";

    for (const auto& zajecie : zajecia) {
        int dostepneMiejsca = uslugiZajec.pobierzDostepneMiejscaZajec(zajecie.pobierzId());

        std::cout << zajecie.pobierzId() << "\t"
            << zajecie.pobierzNazwe() << "\t"
            << zajecie.pobierzTrenera() << "\t"
            << zajecie.pobierzDate() << "\t"
            << zajecie.pobierzCzas() << "\t"
            << zajecie.pobierzCzasTrwania() << " min\t"
            << zajecie.pobierzMaksUczestnikow() << "\t"
            << dostepneMiejsca << "\n";
    }
}

void zarezerwujMiejsceNaZajeciach(UslugiKlienta& uslugiKlienta, UslugiZajec& uslugiZajec) {
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

    int dostepneMiejsca = uslugiZajec.pobierzDostepneMiejscaZajec(idZajec);
    if (dostepneMiejsca <= 0) {
        std::cout << "Brak dostępnych miejsc na tych zajęciach." << std::endl;
        return;
    }

    bool uprawniony = uslugiZajec.czyKlientUprawniony(idKlienta, idZajec);
    if (!uprawniony) {
        std::cout << "Klient nie ma aktywnego karnetu lub nie spełnia innych warunków do zapisu na zajęcia." << std::endl;
        return;
    }

    Rezerwacja rezerwacja(-1, idKlienta, idZajec, "", "potwierdzona");

    try {
        int id = uslugiZajec.dodajRezerwacje(rezerwacja);
        std::cout << "Dodano rezerwację z ID: " << id << std::endl;
        std::cout << "Klient " << klient->pobierzPelneNazwisko() << " został zapisany na zajęcia "
            << zajecia->pobierzNazwe() << " w dniu " << zajecia->pobierzDate()
            << " o godzinie " << zajecia->pobierzCzas() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
    }
}

void generujRaportAktywnosci(UslugiRaportow& uslugiRaportow) {
    int typRaportu;

    std::cout << "\n=== Generowanie raportu aktywności ===\n";
    std::cout << "1. Raport aktywności klientów\n";
    std::cout << "2. Raport popularności zajęć\n";
    std::cout << "3. Raport finansowy\n";
    std::cout << "Wybierz rodzaj raportu: ";
    std::cin >> typRaportu;

    std::string dataOd, dataDo;
    std::cin.ignore();
    std::cout << "Podaj datę początkową (RRRR-MM-DD): ";
    std::getline(std::cin, dataOd);

    std::cout << "Podaj datę końcową (RRRR-MM-DD): ";
    std::getline(std::cin, dataDo);

    try {
        switch (typRaportu) {
        case 1: {
            auto raporty = uslugiRaportow.generujRaportAktywnosciKlienta(dataOd, dataDo);
            std::cout << "\n=== Raport aktywności klientów ===\n";
            if (raporty.empty()) {
                std::cout << "Brak danych dla podanego okresu." << std::endl;
                return;
            }

            std::cout << "Klient\tZajęcia ogółem\tZajęcia anulowane\tNajczęstsze zajęcia\tOstatnia wizyta\n";
            for (const auto& raport : raporty) {
                std::cout << raport.nazwaKlienta << "\t"
                    << raport.lacznaLiczbaZajec << "\t"
                    << raport.liczbaAnulowanychZajec << "\t"
                    << raport.najczestszaZajecia << "\t"
                    << raport.ostatniaWizyta << "\n";
            }
            break;
        }
        case 2: {
            auto raporty = uslugiRaportow.generujRaportPopularnosciZajec(dataOd, dataDo);
            std::cout << "\n=== Raport popularności zajęć ===\n";
            if (raporty.empty()) {
                std::cout << "Brak danych dla podanego okresu." << std::endl;
                return;
            }

            std::cout << "Zajęcia\tTrener\tRezerwacje\tWypełnienie %\n";
            for (const auto& raport : raporty) {
                std::cout << raport.nazwaZajec << "\t"
                    << raport.trener << "\t"
                    << raport.lacznaLiczbaRezerwacji << "\t"
                    << raport.stopienWypelnienia << "%\n";
            }
            break;
        }
        case 3: {
            auto raport = uslugiRaportow.generujRaportFinansowy(dataOd, dataDo);
            std::cout << "\n=== Raport finansowy ===\n";
            std::cout << "Całkowity przychód: " << raport.lacznyPrzychod << " zł\n";
            std::cout << "Liczba sprzedanych karnetów: " << raport.lacznaLiczbaKarnetow << "\n";
            std::cout << "Przychód z karnetów studenckich: " << raport.przychodStudencki << " zł\n";
            std::cout << "Przychód z karnetów standardowych: " << raport.przychodStandardowy << " zł\n";

            std::cout << "\nPrzychody według typów karnetów:\n";
            for (const auto& [typ, przychod] : raport.przychodWgTypuKarnetu) {
                std::cout << typ << ": " << przychod << " zł\n";
            }

            std::cout << "\nLiczba sprzedanych karnetów według typów:\n";
            for (const auto& [typ, liczba] : raport.liczbaKarnetowWgTypu) {
                std::cout << typ << ": " << liczba << "\n";
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
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

  
    try {
        std::locale::global(std::locale("pl_PL.UTF-8"));
    }
    catch (const std::exception&) {
        std::cout << "Nie udało się ustawić lokalnych ustawień. Używanie domyślnych.\n";
    }
    try {
        MenedzerBD menedzerBD("silka_system.db");
        menedzerBD.otworz();

        KlientDAO klientDAO(menedzerBD);
        KarnetDAO karnetDAO(menedzerBD);
        ZajeciaDAO zajeciaDAO(menedzerBD);

        UslugiKlienta uslugiKlienta(klientDAO);
        UslugiKarnetu uslugiKarnetu(karnetDAO);
        UslugiZajec uslugiZajec(zajeciaDAO, uslugiKarnetu);
        UslugiRaportow uslugiRaportow(menedzerBD);

        ImportDanych importDanych(uslugiKlienta, uslugiKarnetu, uslugiZajec);
        EksportDanych eksportDanych(uslugiKlienta, uslugiKarnetu, uslugiZajec);

        try {
            menedzerBD.wykonajZapytanie("INSERT INTO clients (id, first_name, last_name, email, phone, birth_date, registration_date, notes) "
                "VALUES (100, 'Test', 'Testowy', 'test@test.com', '123456789', '2000-01-01', '2025-05-12', 'Test')");
            std::cout << "Testowy rekord dodany pomyślnie!" << std::endl;
        }
        catch (const WyjatekBazyDanych& e) {
            std::cerr << "Błąd dodawania testowego rekordu: " << e.what() << std::endl;
        }

        int wybor;

        do {
            wyswietlMenu();
            std::cin >> wybor;

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
                dodajKarnetDlaKlienta(uslugiKlienta, uslugiKarnetu);
                break;
            case 5:
                wyswietlKarnetyKlienta(uslugiKlienta, uslugiKarnetu);
                break;
            case 6:
                dodajZajeciaGrupowe(uslugiZajec);
                break;
            case 7:
                wyswietlWszystkieZajecia(uslugiZajec);
                break;
            case 8:
                zarezerwujMiejsceNaZajeciach(uslugiKlienta, uslugiZajec);
                break;
            case 9:
                generujRaportAktywnosci(uslugiRaportow);
                break;
            case 10:
                usunKlientaPoId(uslugiKlienta);
                break;
            case 11: {
                int typImportu;
                std::string sciezkaPliku;

                std::cout << "\n=== Import danych z CSV ===\n";
                std::cout << "1. Import klientów\n";
                std::cout << "2. Import karnetów\n";
                std::cout << "3. Import zajęć\n";
                std::cout << "Wybierz typ danych do importu: ";
                std::cin >> typImportu;

                std::cin.ignore();
                std::cout << "Podaj ścieżkę do pliku CSV: ";
                std::getline(std::cin, sciezkaPliku);

                try {
                    switch (typImportu) {
                    case 1: {
                        auto klienci = importDanych.importujKlientowZCSV(sciezkaPliku);
                        importDanych.zapiszZaimportowanychKlientow(klienci);
                        std::cout << "Zaimportowano " << klienci.size() << " klientów." << std::endl;
                        break;
                    }
                    case 2: {
                        auto karnety = importDanych.importujKarnetyZCSV(sciezkaPliku);
                        importDanych.zapiszZaimportowaneKarnety(karnety);
                        std::cout << "Zaimportowano " << karnety.size() << " karnetów." << std::endl;
                        break;
                    }
                    case 3: {
                        auto zajecia = importDanych.importujZajeciaZCSV(sciezkaPliku);
                        importDanych.zapiszZaimportowaneZajecia(zajecia);
                        std::cout << "Zaimportowano " << zajecia.size() << " zajęć." << std::endl;
                        break;
                    }
                    default:
                        std::cout << "Nieprawidłowy typ danych." << std::endl;
                    }
                }
                catch (const WyjatekImportu& e) {
                    std::cerr << "Błąd importu: " << e.what() << std::endl;
                }
                break;
            }

            case 12: {
                int typImportu;
                std::string sciezkaPliku;

                std::cout << "\n=== Import danych z JSON ===\n";
                std::cout << "1. Import klientów\n";
                std::cout << "2. Import karnetów\n";
                std::cout << "3. Import zajęć\n";
                std::cout << "Wybierz typ danych do importu: ";
                std::cin >> typImportu;

                std::cin.ignore();
                std::cout << "Podaj ścieżkę do pliku JSON: ";
                std::getline(std::cin, sciezkaPliku);

                try {
                    switch (typImportu) {
                    case 1: {
                        auto klienci = importDanych.importujKlientowZJSON(sciezkaPliku);
                        importDanych.zapiszZaimportowanychKlientow(klienci);
                        std::cout << "Zaimportowano " << klienci.size() << " klientów." << std::endl;
                        break;
                    }
                    case 2: {
                        auto karnety = importDanych.importujKarnetyZJSON(sciezkaPliku);
                        importDanych.zapiszZaimportowaneKarnety(karnety);
                        std::cout << "Zaimportowano " << karnety.size() << " karnetów." << std::endl;
                        break;
                    }
                    case 3: {
                        auto zajecia = importDanych.importujZajeciaZJSON(sciezkaPliku);
                        importDanych.zapiszZaimportowaneZajecia(zajecia);
                        std::cout << "Zaimportowano " << zajecia.size() << " zajęć." << std::endl;
                        break;
                    }
                    default:
                        std::cout << "Nieprawidłowy typ danych." << std::endl;
                    }
                }
                catch (const WyjatekImportu& e) {
                    std::cerr << "Błąd importu: " << e.what() << std::endl;
                }
                break;
            }
            case 13: {
                int typEksportu;
                std::string sciezkaPliku;

                std::cout << "\n=== Eksport danych do CSV ===\n";
                std::cout << "1. Eksport klientów\n";
                std::cout << "2. Eksport karnetów\n";
                std::cout << "3. Eksport zajęć\n";
                std::cout << "4. Eksport rezerwacji\n";
                std::cout << "Wybierz typ danych do eksportu: ";
                std::cin >> typEksportu;

                std::cin.ignore();
                std::cout << "Podaj ścieżkę do pliku CSV: ";
                std::getline(std::cin, sciezkaPliku);

                try {
                    bool sukces = false;

                    switch (typEksportu) {
                    case 1:
                        sukces = eksportDanych.eksportujKlientowDoCSV(sciezkaPliku);
                        break;
                    case 2:
                        sukces = eksportDanych.eksportujKarnetyDoCSV(sciezkaPliku);
                        break;
                    case 3:
                        sukces = eksportDanych.eksportujZajeciaDoCSV(sciezkaPliku);
                        break;
                    case 4:
                        sukces = eksportDanych.eksportujRezerwacjeDoCSV(sciezkaPliku);
                        break;
                    default:
                        std::cout << "Nieprawidłowy typ danych." << std::endl;
                        break;
                    }

                    if (sukces) {
                        std::cout << "Dane zostały wyeksportowane do pliku: " << sciezkaPliku << std::endl;
                    }
                }
                catch (const WyjatekEksportu& e) {
                    std::cerr << "Błąd eksportu: " << e.what() << std::endl;
                }
                break;
            }
            case 14: {
                int typEksportu;
                std::string sciezkaPliku;

                std::cout << "\n=== Eksport danych do JSON ===\n";
                std::cout << "1. Eksport klientów\n";
                std::cout << "2. Eksport karnetów\n";
                std::cout << "3. Eksport zajęć\n";
                std::cout << "4. Eksport rezerwacji\n";
                std::cout << "Wybierz typ danych do eksportu: ";
                std::cin >> typEksportu;

                std::cin.ignore();
                std::cout << "Podaj ścieżkę do pliku JSON: ";
                std::getline(std::cin, sciezkaPliku);

                try {
                    bool sukces = false;

                    switch (typEksportu) {
                    case 1:
                        sukces = eksportDanych.eksportujKlientowDoJSON(sciezkaPliku);
                        break;
                    case 2:
                        sukces = eksportDanych.eksportujKarnetyDoJSON(sciezkaPliku);
                        break;
                    case 3:
                        sukces = eksportDanych.eksportujZajeciaDoJSON(sciezkaPliku);
                        break;
                    case 4:
                        sukces = eksportDanych.eksportujRezerwacjeDoJSON(sciezkaPliku);
                        break;
                    default:
                        std::cout << "Nieprawidłowy typ danych." << std::endl;
                        break;
                    }

                    if (sukces) {
                        std::cout << "Dane zostały wyeksportowane do pliku: " << sciezkaPliku << std::endl;
                    }
                }
                catch (const WyjatekEksportu& e) {
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
        } while (wybor != 0);

        menedzerBD.zamknij();
    }
    catch (const WyjatekBazyDanych& e) {
        std::cerr << "Błąd bazy danych: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}