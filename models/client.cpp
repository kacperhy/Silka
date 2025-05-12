// models/client.cpp
#include "client.h"

Client::Client() : id(-1), firstName(""), lastName(""), email(""), 
                   phone(""), birthDate(""), registrationDate(getCurrentDate()), 
                   notes("") {
}

Client::Client(int id, const std::string& firstName, const std::string& lastName, 
               const std::string& email, const std::string& phone,
               const std::string& birthDate, const std::string& registrationDate,
               const std::string& notes) : 
    id(id), firstName(firstName), lastName(lastName), email(email),
    phone(phone), birthDate(birthDate), 
    registrationDate(registrationDate.empty() ? getCurrentDate() : registrationDate),
    notes(notes) {
}

int Client::getId() const {
    return id;
}

std::string Client::getFirstName() const {
    return firstName;
}

std::string Client::getLastName() const {
    return lastName;
}

std::string Client::getEmail() const {
    return email;
}

std::string Client::getPhone() const {
    return phone;
}

std::string Client::getBirthDate() const {
    return birthDate;
}

std::string Client::getRegistrationDate() const {
    return registrationDate;
}

std::string Client::getNotes() const {
    return notes;
}

std::string Client::getFullName() const {
    return firstName + " " + lastName;
}

void Client::setId(int id) {
    this->id = id;
}

void Client::setFirstName(const std::string& firstName) {
    this->firstName = firstName;
}

void Client::setLastName(const std::string& lastName) {
    this->lastName = lastName;
}

void Client::setEmail(const std::string& email) {
    this->email = email;
}

void Client::setPhone(const std::string& phone) {
    this->phone = phone;
}

void Client::setBirthDate(const std::string& birthDate) {
    this->birthDate = birthDate;
}

void Client::setRegistrationDate(const std::string& registrationDate) {
    this->registrationDate = registrationDate;
}

void Client::setNotes(const std::string& notes) {
    this->notes = notes;
}

std::string Client::getCurrentDate() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}