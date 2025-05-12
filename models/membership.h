// models/membership.h
#ifndef MEMBERSHIP_H
#define MEMBERSHIP_H

#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

class Membership {
public:
    Membership();
    Membership(int id, int clientId, const std::string& type, 
              const std::string& startDate, const std::string& endDate,
              double price, bool isActive);
    
    // Gettery
    int getId() const;
    int getClientId() const;
    std::string getType() const;
    std::string getStartDate() const;
    std::string getEndDate() const;
    double getPrice() const;
    bool getIsActive() const;
    
    // Settery
    void setId(int id);
    void setClientId(int clientId);
    void setType(const std::string& type);
    void setStartDate(const std::string& startDate);
    void setEndDate(const std::string& endDate);
    void setPrice(double price);
    void setIsActive(bool isActive);
    
    // Metody biznesowe
    bool isValid() const;  // Sprawdza, czy karnet jest ważny na dzień dzisiejszy
    int daysLeft() const;  // Oblicza liczbę dni pozostałych do końca ważności karnetu
    
    // Metoda pomocnicza - zwraca aktualną datę w formacie YYYY-MM-DD
    static std::string getCurrentDate();
    
    // Dodaje określoną liczbę dni do podanej daty (format YYYY-MM-DD)
    static std::string addDaysToDate(const std::string& date, int days);
    
private:
    int id;
    int clientId;
    std::string type;  // 'normal' lub 'student'
    std::string startDate;
    std::string endDate;
    double price;
    bool isActive;
    
    // Konwertuje string w formacie YYYY-MM-DD na obiekt struct tm
    static std::tm stringToDate(const std::string& dateStr);
    
    // Oblicza różnicę dni między dwiema datami
    static int daysBetween(const std::string& date1, const std::string& date2);
};

#endif // MEMBERSHIP_H