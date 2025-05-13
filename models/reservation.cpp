// models/reservation.cpp
#define _CRT_SECURE_NO_WARNINGS
#include "reservation.h"

Reservation::Reservation() : id(-1), clientId(-1), classId(-1),
                            reservationDate(getCurrentDateTime()), status("confirmed") {
}

Reservation::Reservation(int id, int clientId, int classId, 
                        const std::string& reservationDate, const std::string& status) :
    id(id), clientId(clientId), classId(classId),
    reservationDate(reservationDate.empty() ? getCurrentDateTime() : reservationDate),
    status(status.empty() ? "confirmed" : status) {
}

int Reservation::getId() const {
    return id;
}

int Reservation::getClientId() const {
    return clientId;
}

int Reservation::getClassId() const {
    return classId;
}

std::string Reservation::getReservationDate() const {
    return reservationDate;
}

std::string Reservation::getStatus() const {
    return status;
}

void Reservation::setId(int id) {
    this->id = id;
}

void Reservation::setClientId(int clientId) {
    this->clientId = clientId;
}

void Reservation::setClassId(int classId) {
    this->classId = classId;
}

void Reservation::setReservationDate(const std::string& reservationDate) {
    this->reservationDate = reservationDate;
}

void Reservation::setStatus(const std::string& status) {
    this->status = status;
}

bool Reservation::isConfirmed() const {
    return status == "confirmed";
}

std::string Reservation::getCurrentDateTime() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}