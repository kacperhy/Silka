// models/client.h
#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

class Client {
public:
    Client();
    Client(int id, const std::string& firstName, const std::string& lastName, 
           const std::string& email, const std::string& phone,
           const std::string& birthDate, const std::string& registrationDate,
           const std::string& notes);
    
    // Gettery
    int getId() const;
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getEmail() const;
    std::string getPhone() const;
    std::string getBirthDate() const;
    std::string getRegistrationDate() const;
    std::string getNotes() const;
    std::string getFullName() const;
    
    // Settery
    void setId(int id);
    void setFirstName(const std::string& firstName);
    void setLastName(const std::string& lastName);
    void setEmail(const std::string& email);
    void setPhone(const std::string& phone);
    void setBirthDate(const std::string& birthDate);
    void setRegistrationDate(const std::string& registrationDate);
    void setNotes(const std::string& notes);
    
    // Metoda pomocnicza - zwraca aktualną datę w formacie YYYY-MM-DD
    static std::string getCurrentDate();
    
private:
    int id;
    std::string firstName;
    std::string lastName;
    std::string email;
    std::string phone;
    std::string birthDate;
    std::string registrationDate;
    std::string notes;
};

#endif // CLIENT_H