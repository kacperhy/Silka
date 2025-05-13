#define _CRT_SECURE_NO_WARNINGS
#include "karnet.h"
#include <ctime>
#include <algorithm>
#include <stdexcept>

Karnet::Karnet() : id(-1), idKlienta(-1), typ(""),
dataRozpoczecia(pobierzAktualnaDate()), dataZakonczenia(""),
cena(0.0), czyAktywny(true) {
}

Karnet::Karnet(int id, int idKlienta, const std::string& typ,
    const std::string& dataRozpoczecia, const std::string& dataZakonczenia,
    double cena, bool czyAktywny) :
    id(id), idKlienta(idKlienta), typ(typ),
    dataRozpoczecia(dataRozpoczecia.empty() ? pobierzAktualnaDate() : dataRozpoczecia),
    dataZakonczenia(dataZakonczenia), cena(cena), czyAktywny(czyAktywny) {
}

int Karnet::pobierzId() const {
    return id;
}

int Karnet::pobierzIdKlienta() const {
    return idKlienta;
}

std::string Karnet::pobierzTyp() const {
    return typ;
}

std::string Karnet::pobierzDateRozpoczecia() const {
    return dataRozpoczecia;
}

std::string Karnet::pobierzDateZakonczenia() const {
    return dataZakonczenia;
}

double Karnet::pobierzCene() const {
    return cena;
}

bool Karnet::pobierzCzyAktywny() const {
    return czyAktywny;
}

void Karnet::ustawId(int id) {
    this->id = id;
}

void Karnet::ustawIdKlienta(int idKlienta) {
    this->idKlienta = idKlienta;
}

void Karnet::ustawTyp(const std::string& typ) {
    this->typ = typ;
}

void Karnet::ustawDateRozpoczecia(const std::string& dataRozpoczecia) {
    this->dataRozpoczecia = dataRozpoczecia;
}

void Karnet::ustawDateZakonczenia(const std::string& dataZakonczenia) {
    this->dataZakonczenia = dataZakonczenia;
}

void Karnet::ustawCene(double cena) {
    this->cena = cena;
}

void Karnet::ustawCzyAktywny(bool czyAktywny) {
    this->czyAktywny = czyAktywny;
}

bool Karnet::czyWazny() const {
    if (!czyAktywny) return false;

    std::string dzisiaj = pobierzAktualnaDate();
    return dataRozpoczecia <= dzisiaj && dzisiaj <= dataZakonczenia;
}

int Karnet::ileDniPozostalo() const {
    if (!czyWazny()) return 0;

    std::string dzisiaj = pobierzAktualnaDate();
    return dniPomiedzy(dzisiaj, dataZakonczenia);
}

std::string Karnet::pobierzAktualnaDate() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

std::string Karnet::dodajDniDoData(const std::string& data, int dni) {
    std::tm tm = konwertujStringNaDate(data);
    std::time_t t = std::mktime(&tm);
    t += dni * 24 * 60 * 60; 
    tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

std::tm Karnet::konwertujStringNaDate(const std::string& tekstDaty) {
    std::tm tm = {};
    if (sscanf(tekstDaty.c_str(), "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) == 3) {
        tm.tm_year -= 1900;  
        tm.tm_mon -= 1;      
        return tm;
    }
    throw std::runtime_error("Nieprawid³owy format daty: " + tekstDaty);
}

int Karnet::dniPomiedzy(const std::string& data1, const std::string& data2) {
    std::tm tm1 = konwertujStringNaDate(data1);
    std::tm tm2 = konwertujStringNaDate(data2);

    std::time_t t1 = std::mktime(&tm1);
    std::time_t t2 = std::mktime(&tm2);

    return static_cast<int>((t2 - t1) / (24 * 60 * 60));
}