// services/uslugi_zajec.h
#ifndef USLUGI_ZAJEC_H
#define USLUGI_ZAJEC_H

#include <vector>
#include <memory>
#include "../models/zajecia.h"
#include "../models/rezerwacja.h"
#include "../database/dao/zajecia_dao.h"
#include "uslugi_karnetu.h"

class UslugiZajec {
public:
    UslugiZajec(ZajeciaDAO& zajeciaDAO, UslugiKarnetu& uslugiKarnetu);

    // Metody zarz�dzania zaj�ciami
    std::vector<Zajecia> pobierzWszystkieZajecia();
    std::unique_ptr<Zajecia> pobierzZajeciaPoId(int id);
    int dodajZajecia(const Zajecia& zajecia);
    bool aktualizujZajecia(const Zajecia& zajecia);
    bool usunZajecia(int id);

    // Metody rezerwacji
    std::vector<Rezerwacja> pobierzWszystkieRezerwacje();
    std::vector<Rezerwacja> pobierzRezerwacjeKlienta(int idKlienta);
    std::vector<Rezerwacja> pobierzRezerwacjeZajec(int idZajec);
    int dodajRezerwacje(const Rezerwacja& rezerwacja);
    bool anulujRezerwacje(int idRezerwacji);

    // Metody biznesowe
    int pobierzDostepneMiejscaZajec(int idZajec);
    bool czyKlientUprawniony(int idKlienta, int idZajec);

private:
    ZajeciaDAO& zajeciaDAO;
    UslugiKarnetu& uslugiKarnetu;
};

#endif // USLUGI_ZAJEC_H