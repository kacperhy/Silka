// models/membership.cpp
#define _CRT_SECURE_NO_WARNINGS
#include "membership.h"
#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>

Membership::Membership() : id(-1), clientId(-1), type("normal"), startDate(getCurrentDate()),
endDate(addDaysToDate(getCurrentDate(), 30)), price(0.0), isActive(true) {
}

Membership::Membership(int id, int clientId, const std::string& type,
    const std::string& startDate, const std::string& endDate,
    double price, bool isActive) :
    id(id), clientId(clientId), type(type),
    startDate(startDate.empty() ? getCurrentDate() : startDate),
    endDate(endDate.empty() ? addDaysToDate(startDate.empty() ? getCurrentDate() : startDate, 30) : endDate),
    price(price), isActive(isActive) {
}

int Membership::getId() const {
    return id;
}

int Membership::getClientId() const {
    return clientId;
}

std::string Membership::getType() const {
    return type;
}

std::string Membership::getStartDate() const {
    return startDate;
}

std::string Membership::getEndDate() const {
    return endDate;
}

double Membership::getPrice() const {
    return price;
}

bool Membership::getIsActive() const {
    return isActive;
}

void Membership::setId(int id) {
    this->id = id;
}

void Membership::setClientId(int clientId) {
    this->clientId = clientId;
}

void Membership::setType(const std::string& type) {
    this->type = type;
}

void Membership::setStartDate(const std::string& startDate) {
    this->startDate = startDate;
}

void Membership::setEndDate(const std::string& endDate) {
    this->endDate = endDate;
}

void Membership::setPrice(double price) {
    this->price = price;
}

void Membership::setIsActive(bool isActive) {
    this->isActive = isActive;
}

bool Membership::isValid() const {
    return isActive && (daysBetween(getCurrentDate(), endDate) >= 0);
}

int Membership::daysLeft() const {
    if (!isActive) {
        return 0;
    }
    return daysBetween(getCurrentDate(), endDate);
}

std::string Membership::getCurrentDate() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

std::tm Membership::stringToDate(const std::string& dateStr) {
    std::tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    return tm;
}

int Membership::daysBetween(const std::string& date1, const std::string& date2) {
    std::tm tm1 = stringToDate(date1);
    std::tm tm2 = stringToDate(date2);

    std::time_t time1 = std::mktime(&tm1);
    std::time_t time2 = std::mktime(&tm2);

    // Ró¿nica w sekundach
    double seconds = std::difftime(time2, time1);

    // Konwersja na dni (1 dzieñ = 86400 sekund)
    return static_cast<int>(seconds / 86400);
}

std::string Membership::addDaysToDate(const std::string& date, int days) {
    std::tm tm = stringToDate(date);
    std::time_t time = std::mktime(&tm);

    // Dodaj dni (1 dzieñ = 86400 sekund)
    time += days * 86400;

    tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}