// models/membership.h (przemianowany na models/karnet.h)
#ifndef KARNET_H
#define KARNET_H

#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

class Karnet {
public:
    Karnet();
    Karnet(int id, int idKlienta, const std::string& typ,
        const std::string& dataRozpoczecia, const std::string& dataZakonczenia,
        double cena, bool czyAktywny);

    // Gettery
    int pobierzId() const;
    int pobierzIdKlienta() const;
    std::string pobierzTyp() const;
    std::string pobierzDateRozpoczecia() const;
    std::string pobierzDateZakonczenia() const;
    double pobierzCene() const;
    bool pobierzCzyAktywny() const;

    // Settery
    void ustawId(int id);
    void ustawIdKlienta(int idKlienta);
    void ustawTyp(const std::string& typ);
    void ustawDateRozpoczecia(const std::string& dataRozpoczecia);
    void ustawDateZakonczenia(const std::string& dataZakonczenia);
    void ustawCene(double cena);
    void ustawCzyAktywny(bool czyAktywny);

    // Metody biznesowe
    bool czyWazny() const;  // Sprawdza, czy karnet jest ważny na dzień dzisiejszy
    int ileDniPozostalo() const;  // Oblicza liczbę dni pozostałych do końca ważności karnetu

    // Metoda pomocnicza - zwraca aktualną datę w formacie RRRR-MM-DD
    static std::string pobierzAktualnaDate();

    // Dodaje określoną liczbę dni do podanej daty (format RRRR-MM-DD)
    static std::string dodajDniDoData(const std::string& data, int dni);

private:
    int id;
    int idKlienta;
    std::string typ;  // 'normalny' lub 'studencki'
    std::string dataRozpoczecia;
    std::string dataZakonczenia;
    double cena;
    bool czyAktywny;

    // Konwertuje string w formacie RRRR-MM-DD na obiekt struct tm
    static std::tm konwertujStringNaDate(const std::string& tekstDaty);

    // Oblicza różnicę dni między dwiema datami
    static int dniPomiedzy(const std::string& data1, const std::string& data2);
};

#endif // KARNET_H