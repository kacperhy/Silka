// database/dao/karnet_dao.cpp
#include "karnet_dao.h"
#include <sstream>

KarnetDAO::KarnetDAO(MenedzerBD& menedzerBD) : menedzerBD(menedzerBD) {
}

std::vector<Karnet> KarnetDAO::pobierzWszystkie() {
    std::vector<Karnet> karnety;

    auto dane = menedzerBD.pobierzDane("SELECT * FROM memberships ORDER BY start_date DESC");

    for (const auto& wiersz : dane) {
        karnety.push_back(utworzKarnetZWiersza(wiersz));
    }

    return karnety;
}

std::unique_ptr<Karnet> KarnetDAO::pobierzPoId(int id) {
    std::stringstream zapytanie;
    zapytanie << "SELECT * FROM memberships WHERE id = " << id;

    auto dane = menedzerBD.pobierzDane(zapytanie.str());

    if (dane.empty()) {
        return nullptr;
    }

    return std::make_unique<Karnet>(utworzKarnetZWiersza(dane[0]));
}

std::vector<Karnet> KarnetDAO::pobierzDlaKlienta(int idKlienta) {
    std::vector<Karnet> karnety;

    std::stringstream zapytanie;
    zapytanie << "SELECT * FROM memberships WHERE client_id = " << idKlienta << " ORDER BY start_date DESC";

    auto dane = menedzerBD.pobierzDane(zapytanie.str());

    for (const auto& wiersz : dane) {
        karnety.push_back(utworzKarnetZWiersza(wiersz));
    }

    return karnety;
}

int KarnetDAO::dodaj(const Karnet& karnet) {
    std::stringstream zapytanie;
    zapytanie << "INSERT INTO memberships (client_id, type, start_date, end_date, price, is_active) VALUES ("
        << karnet.pobierzIdKlienta() << ", "
        << "'" << karnet.pobierzTyp() << "', "
        << "'" << karnet.pobierzDateRozpoczecia() << "', "
        << "'" << karnet.pobierzDateZakonczenia() << "', "
        << karnet.pobierzCene() << ", "
        << (karnet.pobierzCzyAktywny() ? 1 : 0) << ")";

    return menedzerBD.wykonajZapytanieZwracajaceId(zapytanie.str());
}

bool KarnetDAO::aktualizuj(const Karnet& karnet) {
    std::stringstream zapytanie;
    zapytanie << "UPDATE memberships SET "
        << "client_id = " << karnet.pobierzIdKlienta() << ", "
        << "type = '" << karnet.pobierzTyp() << "', "
        << "start_date = '" << karnet.pobierzDateRozpoczecia() << "', "
        << "end_date = '" << karnet.pobierzDateZakonczenia() << "', "
        << "price = " << karnet.pobierzCene() << ", "
        << "is_active = " << (karnet.pobierzCzyAktywny() ? 1 : 0) << " "
        << "WHERE id = " << karnet.pobierzId();

    try {
        menedzerBD.wykonajZapytanie(zapytanie.str());
        return true;
    }
    catch (const WyjatekBazyDanych&) {
        return false;
    }
}

bool KarnetDAO::usun(int id) {
    std::stringstream zapytanie;
    zapytanie << "DELETE FROM memberships WHERE id = " << id;

    try {
        menedzerBD.wykonajZapytanie(zapytanie.str());
        return true;
    }
    catch (const WyjatekBazyDanych&) {
        return false;
    }
}

bool KarnetDAO::czyKlientMaAktywnyKarnet(int idKlienta) {
    std::string aktualnaDdata = Karnet::pobierzAktualnaDate();

    std::stringstream zapytanie;
    zapytanie << "SELECT COUNT(*) FROM memberships "
        << "WHERE client_id = " << idKlienta << " "
        << "AND is_active = 1 "
        << "AND start_date <= '" << aktualnaDdata << "' "
        << "AND end_date >= '" << aktualnaDdata << "'";

    std::string wynik = menedzerBD.pobierzWartosc(zapytanie.str());

    return std::stoi(wynik) > 0;
}

Karnet KarnetDAO::utworzKarnetZWiersza(const WierszBD& wiersz) {
    if (wiersz.size() < 7) {
        throw WyjatekBazyDanych("Nieprawid³owa liczba kolumn dla karnetu");
    }

    int id = std::stoi(wiersz[0]);
    int idKlienta = std::stoi(wiersz[1]);
    std::string typ = wiersz[2];
    std::string dataRozpoczecia = wiersz[3];
    std::string dataZakonczenia = wiersz[4];
    double cena = std::stod(wiersz[5]);
    bool czyAktywny = (wiersz[6] == "1" || wiersz[6] == "true");

    return Karnet(id, idKlienta, typ, dataRozpoczecia, dataZakonczenia, cena, czyAktywny);
}