// database/menedzer_bd.cpp
#include "menedzer_bd.h"
#include <iostream>

MenedzerBD::MenedzerBD(const std::string& sciezkaPliku)
    : sciezkaPliku(sciezkaPliku), polaczenie(nullptr) {
}

MenedzerBD::~MenedzerBD() {
    if (czyOtwarta()) {
        zamknij();
    }
}

void MenedzerBD::otworz() {
    if (czyOtwarta()) {
        return;
    }

    int rc = sqlite3_open(sciezkaPliku.c_str(), &polaczenie);
    sprawdzBlad(rc, "Otwieranie bazy danych");

    // Automatycznie inicjalizuj bazê danych przy otwarciu
    inicjalizujBazeDanych();
}

void MenedzerBD::zamknij() {
    if (!czyOtwarta()) {
        return;
    }

    int rc = sqlite3_close(polaczenie);
    sprawdzBlad(rc, "Zamykanie bazy danych");
    polaczenie = nullptr;
}

bool MenedzerBD::czyOtwarta() const {
    return polaczenie != nullptr;
}

void MenedzerBD::wykonajZapytanie(const std::string& zapytanie) {
    if (!czyOtwarta()) {
        throw WyjatekBazyDanych("Baza danych nie jest otwarta");
    }

    char* komunikatBledu = nullptr;
    int rc = sqlite3_exec(polaczenie, zapytanie.c_str(), nullptr, nullptr, &komunikatBledu);

    if (rc != SQLITE_OK) {
        std::string blad = "B³¹d SQL: ";
        if (komunikatBledu) {
            blad += komunikatBledu;
            sqlite3_free(komunikatBledu);
        }
        throw WyjatekBazyDanych(blad);
    }
}

int MenedzerBD::wykonajZapytanieZwracajaceId(const std::string& zapytanie) {
    wykonajZapytanie(zapytanie);
    return static_cast<int>(sqlite3_last_insert_rowid(polaczenie));
}

TabelaBD MenedzerBD::pobierzDane(const std::string& zapytanie) {
    if (!czyOtwarta()) {
        throw WyjatekBazyDanych("Baza danych nie jest otwarta");
    }

    TabelaBD wyniki;
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(polaczenie, zapytanie.c_str(), -1, &stmt, nullptr);
    sprawdzBlad(rc, "Przygotowanie zapytania");

    int kolumny = sqlite3_column_count(stmt);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        WierszBD wiersz;

        for (int i = 0; i < kolumny; i++) {
            const unsigned char* wartosc = sqlite3_column_text(stmt, i);

            if (wartosc) {
                wiersz.push_back(reinterpret_cast<const char*>(wartosc));
            }
            else {
                wiersz.push_back("");
            }
        }

        wyniki.push_back(wiersz);
    }

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sprawdzBlad(rc, "Pobieranie danych");
    }

    sqlite3_finalize(stmt);
    return wyniki;
}

WierszBD MenedzerBD::pobierzWiersz(const std::string& zapytanie) {
    TabelaBD wyniki = pobierzDane(zapytanie);

    if (wyniki.empty()) {
        return WierszBD();
    }

    return wyniki[0];
}

std::string MenedzerBD::pobierzWartosc(const std::string& zapytanie) {
    WierszBD wiersz = pobierzWiersz(zapytanie);

    if (wiersz.empty()) {
        return "";
    }

    return wiersz[0];
}

TabelaBD MenedzerBD::pobierzDaneZParametrami(const std::string& zapytanie,
    const std::vector<ParamZapytania>& parametry) {
    if (!czyOtwarta()) {
        throw WyjatekBazyDanych("Baza danych nie jest otwarta");
    }

    TabelaBD wyniki;
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(polaczenie, zapytanie.c_str(), -1, &stmt, nullptr);
    sprawdzBlad(rc, "Przygotowanie zapytania");

    bindParameters(stmt, parametry);

    int kolumny = sqlite3_column_count(stmt);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        WierszBD wiersz;

        for (int i = 0; i < kolumny; i++) {
            const unsigned char* wartosc = sqlite3_column_text(stmt, i);

            if (wartosc) {
                wiersz.push_back(reinterpret_cast<const char*>(wartosc));
            }
            else {
                wiersz.push_back("");
            }
        }

        wyniki.push_back(wiersz);
    }

    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sprawdzBlad(rc, "Pobieranie danych");
    }

    sqlite3_finalize(stmt);
    return wyniki;
}

WierszBD MenedzerBD::pobierzWierszZParametrami(const std::string& zapytanie,
    const std::vector<ParamZapytania>& parametry) {
    TabelaBD wyniki = pobierzDaneZParametrami(zapytanie, parametry);

    if (wyniki.empty()) {
        return WierszBD();
    }

    return wyniki[0];
}

std::string MenedzerBD::pobierzWartoscZParametrami(const std::string& zapytanie,
    const std::vector<ParamZapytania>& parametry) {
    WierszBD wiersz = pobierzWierszZParametrami(zapytanie, parametry);

    if (wiersz.empty()) {
        return "";
    }

    return wiersz[0];
}

void MenedzerBD::inicjalizujBazeDanych() {
    // Tworzenie tabel w bazie danych (jeœli nie istniej¹)
    wykonajZapytanie(
        "CREATE TABLE IF NOT EXISTS clients ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "first_name TEXT NOT NULL,"
        "last_name TEXT NOT NULL,"
        "email TEXT,"
        "phone TEXT,"
        "birth_date TEXT,"
        "registration_date TEXT NOT NULL,"
        "notes TEXT"
        ");"
    );

    wykonajZapytanie(
        "CREATE TABLE IF NOT EXISTS memberships ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "client_id INTEGER NOT NULL,"
        "type TEXT NOT NULL,"
        "start_date TEXT NOT NULL,"
        "end_date TEXT NOT NULL,"
        "price REAL NOT NULL,"
        "is_active INTEGER NOT NULL DEFAULT 1,"
        "FOREIGN KEY (client_id) REFERENCES clients(id)"
        ");"
    );

    wykonajZapytanie(
        "CREATE TABLE IF NOT EXISTS gym_classes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "trainer TEXT NOT NULL,"
        "max_participants INTEGER NOT NULL,"
        "date TEXT NOT NULL,"
        "time TEXT NOT NULL,"
        "duration INTEGER NOT NULL,"
        "description TEXT"
        ");"
    );

    wykonajZapytanie(
        "CREATE TABLE IF NOT EXISTS reservations ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "client_id INTEGER NOT NULL,"
        "class_id INTEGER NOT NULL,"
        "reservation_date TEXT NOT NULL,"
        "status TEXT NOT NULL,"
        "FOREIGN KEY (client_id) REFERENCES clients(id),"
        "FOREIGN KEY (class_id) REFERENCES gym_classes(id)"
        ");"
    );
}

void MenedzerBD::sprawdzBlad(int kod, const std::string& operacja) {
    if (kod != SQLITE_OK && kod != SQLITE_DONE) {
        std::string komunikat = "B³¹d bazy danych podczas " + operacja + ": ";
        if (polaczenie) {
            komunikat += sqlite3_errmsg(polaczenie);
        }
        else {
            komunikat += "po³¹czenie nie zosta³o ustanowione";
        }
        throw WyjatekBazyDanych(komunikat);
    }
}

void MenedzerBD::bindParameters(sqlite3_stmt* stmt, const std::vector<ParamZapytania>& parametry) {
    for (const auto& param : parametry) {
        int index = param.first;
        const std::string& wartosc = param.second;

        int rc = sqlite3_bind_text(stmt, index, wartosc.c_str(), -1, SQLITE_TRANSIENT);
        sprawdzBlad(rc, "Bindowanie parametrów");
    }
}