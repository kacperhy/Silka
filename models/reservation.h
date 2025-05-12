// models/reservation.h
#ifndef RESERVATION_H
#define RESERVATION_H

#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

class Reservation {
public:
    Reservation();
    Reservation(int id, int clientId, int classId, 
               const std::string& reservationDate, const std::string& status);
    
    // Gettery
    int getId() const;
    int getClientId() const;
    int getClassId() const;
    std::string getReservationDate() const;
    std::string getStatus() const;
    
    // Settery
    void setId(int id);
    void setClientId(int clientId);
    void setClassId(int classId);
    void setReservationDate(const std::string& reservationDate);
    void setStatus(const std::string& status);
    
    // Metody biznesowe
    bool isConfirmed() const;
    
    // Metoda pomocnicza - zwraca aktualną datę i czas w formacie YYYY-MM-DD HH:MM:SS
    static std::string getCurrentDateTime();
    
private:
    int id;
    int clientId;
    int classId;
    std::string reservationDate;
    std::string status;  // 'confirmed' lub 'cancelled'
};

#endif // RESERVATION_H