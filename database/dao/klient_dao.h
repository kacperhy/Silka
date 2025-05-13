// database/dao/klient_dao.h
#ifndef KLIENT_DAO_H
#define KLIENT_DAO_H

#include <vector>
#include <memory>
#include "../../models/klient.h"
#include "../menedzer_bd.h"

class KlientDAO {
public:
    explicit KlientDAO(MenedzerBD& menedzerBD);

    // Metody CRUD
    std::vector<Klient> pobierzWszystkich();
    std::unique_ptr<Klient> pobierzPoId(int id);
    int dodaj(const Klient& klient);
    bool aktualizuj(const Klient& klient);
    bool usun(int id);

    // Metody wyszukiwania
    std::vector<Klient> wyszukaj(const std::string& klucz);

private:
    MenedzerBD& menedzerBD;

    // Metody pomocnicze
    Klient utworzKlientaZWiersza(const WierszBD& wiersz);
};

#endif // KLIENT_DAO_H