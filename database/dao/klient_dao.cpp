// database/dao/klient_dao.cpp
#include "klient_dao.h"
#include <sstream>

KlientDAO::KlientDAO(MenedzerBD& menedzerBD) : menedzerBD(menedzerBD) {
}

std::vector<Klient> KlientDAO::pobierzWszystkich() {
    std::vector<Klient> klienci;

    auto dane = menedzerBD.pobierzDane("SELECT * FROM clients ORDER BY last_name, first_name");

    for (const auto& wiersz : dane) {
        klienci.push_back(utworzKlientaZWiersza(wiersz));
    }

    return klienci;
}

std::unique_ptr<Klient> KlientDAO::pobierzPoId(int id) {
    std::stringstream zapytanie;
    zapytanie << "SELECT * FROM clients WHERE id = " << id;

    auto dane = menedzerBD.pobierzDane(zapytanie.str());

    if (dane.empty()) {
        return nullptr;
    }

    return std::make_unique<Klient>(utworzKlientaZWiersza(dane[0]));
}

int KlientDAO::dodaj(const Klient& klient) {
    std::stringstream zapytanie;
    zapytanie << "INSERT INTO clients (first_name, last_name, email, phone, birth_date, registration_date, notes) VALUES ("
        << "'" << klient.pobierzImie() << "', "
        << "'" << klient.pobierzNazwisko() << "', "
        << "'" << klient.pobierzEmail() << "', "
        << "'" << klient.pobierzTelefon() << "', "
        << "'" << klient.pobierzDateUrodzenia() << "', "
        << "'" << klient.pobierzDateRejestracji() << "', "
        << "'" << klient.pobierzUwagi() << "')";

    return menedzerBD.wykonajZapytanieZwracajaceId(zapytanie.str());
}

bool KlientDAO::aktualizuj(const Klient& klient) {
    std::stringstream zapytanie;
    zapytanie << "UPDATE clients SET "
        << "first_name = '" << klient.pobierzImie() << "', "
        << "last_name = '" << klient.pobierzNazwisko() << "', "
        << "email = '" << klient.pobierzEmail() << "', "
        << "phone = '" << klient.pobierzTelefon() << "', "
        << "birth_date = '" << klient.pobierzDateUrodzenia() << "', "
        << "registration_date = '" << klient.pobierzDateRejestracji() << "', "
        << "notes = '" << klient.pobierzUwagi() << "' "
        << "WHERE id = " << klient.pobierzId();

    try {
        menedzerBD.wykonajZapytanie(zapytanie.str());
        return true;
    }
    catch (const WyjatekBazyDanych&) {
        return false;
    }
}

bool KlientDAO::usun(int id) {
    std::stringstream zapytanie;
    zapytanie << "DELETE FROM clients WHERE id = " << id;

    try {
        menedzerBD.wykonajZapytanie(zapytanie.str());
        return true;
    }
    catch (const WyjatekBazyDanych&) {
        return false;
    }
}

std::vector<Klient> KlientDAO::wyszukaj(const std::string& klucz) {
    std::vector<Klient> klienci;

    std::stringstream zapytanie;
    zapytanie << "SELECT * FROM clients WHERE "
        << "first_name LIKE '%" << klucz << "%' OR "
        << "last_name LIKE '%" << klucz << "%' OR "
        << "email LIKE '%" << klucz << "%' OR "
        << "phone LIKE '%" << klucz << "%' "
        << "ORDER BY last_name, first_name";

    auto dane = menedzerBD.pobierzDane(zapytanie.str());

    for (const auto& wiersz : dane) {
        klienci.push_back(utworzKlientaZWiersza(wiersz));
    }

    return klienci;
}

Klient KlientDAO::utworzKlientaZWiersza(const WierszBD& wiersz) {
    if (wiersz.size() < 8) {
        throw WyjatekBazyDanych("Nieprawid³owa liczba kolumn dla klienta");
    }

    int id = std::stoi(wiersz[0]);
    std::string imie = wiersz[1];
    std::string nazwisko = wiersz[2];
    std::string email = wiersz[3];
    std::string telefon = wiersz[4];
    std::string dataUrodzenia = wiersz[5];
    std::string dataRejestracji = wiersz[6];
    std::string uwagi = wiersz[7];

    return Klient(id, imie, nazwisko, email, telefon, dataUrodzenia, dataRejestracji, uwagi);
}