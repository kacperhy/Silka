// database/dao/zajecia_dao.cpp
#include "zajecia_dao.h"
#include <sstream>

ZajeciaDAO::ZajeciaDAO(MenedzerBD& menedzerBD) : menedzerBD(menedzerBD) {
}

std::vector<Zajecia> ZajeciaDAO::pobierzWszystkieZajecia() {
    std::vector<Zajecia> zajecia;

    auto dane = menedzerBD.pobierzDane("SELECT * FROM gym_classes ORDER BY date, time");

    for (const auto& wiersz : dane) {
        zajecia.push_back(utworzZajeciaZWiersza(wiersz));
    }

    return zajecia;
}

std::unique_ptr<Zajecia> ZajeciaDAO::pobierzZajeciaPoId(int id) {
    std::stringstream zapytanie;
    zapytanie << "SELECT * FROM gym_classes WHERE id = " << id;

    auto dane = menedzerBD.pobierzDane(zapytanie.str());

    if (dane.empty()) {
        return nullptr;
    }

    return std::make_unique<Zajecia>(utworzZajeciaZWiersza(dane[0]));
}

int ZajeciaDAO::dodajZajecia(const Zajecia& zajecia) {
    std::stringstream zapytanie;
    zapytanie << "INSERT INTO gym_classes (name, trainer, max_participants, date, time, duration, description) VALUES ("
        << "'" << zajecia.pobierzNazwe() << "', "
        << "'" << zajecia.pobierzTrenera() << "', "
        << zajecia.pobierzMaksUczestnikow() << ", "
        << "'" << zajecia.pobierzDate() << "', "
        << "'" << zajecia.pobierzCzas() << "', "
        << zajecia.pobierzCzasTrwania() << ", "
        << "'" << zajecia.pobierzOpis() << "')";

    return menedzerBD.wykonajZapytanieZwracajaceId(zapytanie.str());
}

bool ZajeciaDAO::aktualizujZajecia(const Zajecia& zajecia) {
    std::stringstream zapytanie;
    zapytanie << "UPDATE gym_classes SET "
        << "name = '" << zajecia.pobierzNazwe() << "', "
        << "trainer = '" << zajecia.pobierzTrenera() << "', "
        << "max_participants = " << zajecia.pobierzMaksUczestnikow() << ", "
        << "date = '" << zajecia.pobierzDate() << "', "
        << "time = '" << zajecia.pobierzCzas() << "', "
        << "duration = " << zajecia.pobierzCzasTrwania() << ", "
        << "description = '" << zajecia.pobierzOpis() << "' "
        << "WHERE id = " << zajecia.pobierzId();

    try {
        menedzerBD.wykonajZapytanie(zapytanie.str());
        return true;
    }
    catch (const WyjatekBazyDanych&) {
        return false;
    }
}

bool ZajeciaDAO::usunZajecia(int id) {
    std::stringstream zapytanie;
    zapytanie << "DELETE FROM gym_classes WHERE id = " << id;

    try {
        menedzerBD.wykonajZapytanie(zapytanie.str());
        return true;
    }
    catch (const WyjatekBazyDanych&) {
        return false;
    }
}

std::vector<Rezerwacja> ZajeciaDAO::pobierzWszystkieRezerwacje() {
    std::vector<Rezerwacja> rezerwacje;

    auto dane = menedzerBD.pobierzDane("SELECT * FROM reservations ORDER BY reservation_date DESC");

    for (const auto& wiersz : dane) {
        rezerwacje.push_back(utworzRezerwacjeZWiersza(wiersz));
    }

    return rezerwacje;
}

std::vector<Rezerwacja> ZajeciaDAO::pobierzRezerwacjeKlienta(int idKlienta) {
    std::vector<Rezerwacja> rezerwacje;

    std::stringstream zapytanie;
    zapytanie << "SELECT * FROM reservations WHERE client_id = " << idKlienta << " ORDER BY reservation_date DESC";

    auto dane = menedzerBD.pobierzDane(zapytanie.str());

    for (const auto& wiersz : dane) {
        rezerwacje.push_back(utworzRezerwacjeZWiersza(wiersz));
    }

    return rezerwacje;
}

std::vector<Rezerwacja> ZajeciaDAO::pobierzRezerwacjeZajec(int idZajec) {
    std::vector<Rezerwacja> rezerwacje;

    std::stringstream zapytanie;
    zapytanie << "SELECT * FROM reservations WHERE class_id = " << idZajec << " ORDER BY reservation_date";

    auto dane = menedzerBD.pobierzDane(zapytanie.str());

    for (const auto& wiersz : dane) {
        rezerwacje.push_back(utworzRezerwacjeZWiersza(wiersz));
    }

    return rezerwacje;
}

int ZajeciaDAO::dodajRezerwacje(const Rezerwacja& rezerwacja) {
    std::stringstream zapytanie;
    zapytanie << "INSERT INTO reservations (client_id, class_id, reservation_date, status) VALUES ("
        << rezerwacja.pobierzIdKlienta() << ", "
        << rezerwacja.pobierzIdZajec() << ", "
        << "'" << rezerwacja.pobierzDateRezerwacji() << "', "
        << "'" << rezerwacja.pobierzStatus() << "')";

    return menedzerBD.wykonajZapytanieZwracajaceId(zapytanie.str());
}

bool ZajeciaDAO::aktualizujRezerwacje(const Rezerwacja& rezerwacja) {
    std::stringstream zapytanie;
    zapytanie << "UPDATE reservations SET "
        << "client_id = " << rezerwacja.pobierzIdKlienta() << ", "
        << "class_id = " << rezerwacja.pobierzIdZajec() << ", "
        << "reservation_date = '" << rezerwacja.pobierzDateRezerwacji() << "', "
        << "status = '" << rezerwacja.pobierzStatus() << "' "
        << "WHERE id = " << rezerwacja.pobierzId();

    try {
        menedzerBD.wykonajZapytanie(zapytanie.str());
        return true;
    }
    catch (const WyjatekBazyDanych&) {
        return false;
    }
}

bool ZajeciaDAO::usunRezerwacje(int id) {
    std::stringstream zapytanie;
    zapytanie << "DELETE FROM reservations WHERE id = " << id;

    try {
        menedzerBD.wykonajZapytanie(zapytanie.str());
        return true;
    }
    catch (const WyjatekBazyDanych&) {
        return false;
    }
}

int ZajeciaDAO::policzRezerwacjeZajec(int idZajec) {
    std::stringstream zapytanie;
    zapytanie << "SELECT COUNT(*) FROM reservations "
        << "WHERE class_id = " << idZajec << " "
        << "AND status = 'potwierdzona'";

    std::string wynik = menedzerBD.pobierzWartosc(zapytanie.str());

    return std::stoi(wynik);
}

Zajecia ZajeciaDAO::utworzZajeciaZWiersza(const WierszBD& wiersz) {
    if (wiersz.size() < 8) {
        throw WyjatekBazyDanych("Nieprawid³owa liczba kolumn dla zajêæ");
    }

    int id = std::stoi(wiersz[0]);
    std::string nazwa = wiersz[1];
    std::string trener = wiersz[2];
    int maksUczestnikow = std::stoi(wiersz[3]);
    std::string data = wiersz[4];
    std::string czas = wiersz[5];
    int czasTrwania = std::stoi(wiersz[6]);
    std::string opis = wiersz[7];

    return Zajecia(id, nazwa, trener, maksUczestnikow, data, czas, czasTrwania, opis);
}

Rezerwacja ZajeciaDAO::utworzRezerwacjeZWiersza(const WierszBD& wiersz) {
    if (wiersz.size() < 5) {
        throw WyjatekBazyDanych("Nieprawid³owa liczba kolumn dla rezerwacji");
    }

    int id = std::stoi(wiersz[0]);
    int idKlienta = std::stoi(wiersz[1]);
    int idZajec = std::stoi(wiersz[2]);
    std::string dataRezerwacji = wiersz[3];
    std::string status = wiersz[4];

    return Rezerwacja(id, idKlienta, idZajec, dataRezerwacji, status);
}