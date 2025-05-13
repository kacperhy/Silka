// services/client_service.h (przemianowany na services/uslugi_klienta.h)
#ifndef USLUGI_KLIENTA_H
#define USLUGI_KLIENTA_H

#include <vector>
#include <memory>
#include <string>
#include "../models/klient.h"
#include "../database/dao/klient_dao.h"

class UslugiKlienta {
public:
    explicit UslugiKlienta(KlientDAO& klientDAO);

    // Metody biznesowe
    std::vector<Klient> pobierzWszystkichKlientow();
    std::unique_ptr<Klient> pobierzKlientaPoId(int id);
    int dodajKlienta(const Klient& klient);
    bool aktualizujKlienta(const Klient& klient);
    bool usunKlienta(int id);
    std::vector<Klient> wyszukajKlientow(const std::string& klucz);

private:
    KlientDAO& klientDAO;
};

#endif // USLUGI_KLIENTA_H