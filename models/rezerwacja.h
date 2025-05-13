// models/reservation.h (przemianowany na models/rezerwacja.h)
#ifndef REZERWACJA_H
#define REZERWACJA_H

#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

class Rezerwacja {
public:
    Rezerwacja();
    Rezerwacja(int id, int idKlienta, int idZajec,
        const std::string& dataRezerwacji, const std::string& status);

    // Gettery
    int pobierzId() const;
    int pobierzIdKlienta() const;
    int pobierzIdZajec() const;
    std::string pobierzDateRezerwacji() const;
    std::string pobierzStatus() const;

    // Settery
    void ustawId(int id);
    void ustawIdKlienta(int idKlienta);
    void ustawIdZajec(int idZajec);
    void ustawDateRezerwacji(const std::string& dataRezerwacji);
    void ustawStatus(const std::string& status);

    // Metody biznesowe
    bool czyPotwierdzona() const;

    // Metoda pomocnicza - zwraca aktualną datę i czas w formacie RRRR-MM-DD GG:MM:SS
    static std::string pobierzAktualnyCzas();

private:
    int id;
    int idKlienta;
    int idZajec;
    std::string dataRezerwacji;
    std::string status;  // 'potwierdzona' lub 'anulowana'
};

#endif // REZERWACJA_H