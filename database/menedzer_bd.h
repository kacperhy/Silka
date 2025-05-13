// database/menedzer_bd.h
#ifndef MENEDZER_BD_H
#define MENEDZER_BD_H

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <sqlite3.h>

// Typy danych dla wyników zapytañ
using WierszBD = std::vector<std::string>;
using TabelaBD = std::vector<WierszBD>;
using ParamZapytania = std::pair<int, std::string>;

class WyjatekBazyDanych : public std::runtime_error {
public:
    explicit WyjatekBazyDanych(const std::string& wiadomosc) : std::runtime_error(wiadomosc) {}
};

class MenedzerBD {
public:
    explicit MenedzerBD(const std::string& sciezkaPliku);
    ~MenedzerBD();

    // Metody zarz¹dzania po³¹czeniem
    void otworz();
    void zamknij();
    bool czyOtwarta() const;

    // Metody wykonywania zapytañ
    void wykonajZapytanie(const std::string& zapytanie);
    int wykonajZapytanieZwracajaceId(const std::string& zapytanie);

    // Metody pobierania danych
    TabelaBD pobierzDane(const std::string& zapytanie);
    WierszBD pobierzWiersz(const std::string& zapytanie);
    std::string pobierzWartosc(const std::string& zapytanie);

    // Metody z parametrami
    TabelaBD pobierzDaneZParametrami(const std::string& zapytanie,
        const std::vector<ParamZapytania>& parametry);
    WierszBD pobierzWierszZParametrami(const std::string& zapytanie,
        const std::vector<ParamZapytania>& parametry);
    std::string pobierzWartoscZParametrami(const std::string& zapytanie,
        const std::vector<ParamZapytania>& parametry);

    // Inicjalizacja bazy danych
    void inicjalizujBazeDanych();

private:
    std::string sciezkaPliku;
    sqlite3* polaczenie;

    // Metody pomocnicze
    void sprawdzBlad(int kod, const std::string& operacja);
    void bindParameters(sqlite3_stmt* stmt, const std::vector<ParamZapytania>& parametry);
};

#endif // MENEDZER_BD_H