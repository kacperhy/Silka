// services/uslugi_karnetu.h
#ifndef USLUGI_KARNETU_H
#define USLUGI_KARNETU_H

#include <vector>
#include <memory>
#include <string>
#include "../models/karnet.h"
#include "../database/dao/karnet_dao.h"

class UslugiKarnetu {
public:
    explicit UslugiKarnetu(KarnetDAO& karnetDAO);

    // Metody biznesowe
    std::vector<Karnet> pobierzWszystkieKarnety();
    std::unique_ptr<Karnet> pobierzKarnetPoId(int id);
    std::vector<Karnet> pobierzKarnetyKlienta(int idKlienta);
    int dodajKarnet(const Karnet& karnet);
    bool aktualizujKarnet(const Karnet& karnet);
    bool usunKarnet(int id);

    // Fabryki karnetów
    Karnet utworzKarnetMiesieczny(int idKlienta, bool czyStudent);
    Karnet utworzKarnetKwartalny(int idKlienta, bool czyStudent);
    Karnet utworzKarnetRoczny(int idKlienta, bool czyStudent);

    // Metody pomocnicze
    double obliczCene(const std::string& typ, bool czyStudent);
    std::string pobierzTypKarnetu(const std::string& bazaTypu, bool czyStudent);

private:
    KarnetDAO& karnetDAO;

    // Ceny karnetów
    const double CENA_MIESIECZNY = 150.0;
    const double CENA_KWARTALNY = 400.0;
    const double CENA_ROCZNY = 1200.0;

    // Zni¿ka studencka (w procentach)
    const double ZNIZKA_STUDENCKA = 20.0;
};

#endif // USLUGI_KARNETU_H