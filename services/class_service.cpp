// services/class_service.cpp
#include "class_service.h"
#include <stdexcept>

ClassService::ClassService(ClassDAO& classDao, MembershipService& membershipService)
    : classDao(classDao), membershipService(membershipService) {
}

std::vector<GymClass> ClassService::getAllClasses() {
    return classDao.getAllClasses();
}

std::vector<GymClass> ClassService::getUpcomingClasses() {
    return classDao.getUpcomingClasses();
}

std::unique_ptr<GymClass> ClassService::getClassById(int id) {
    return classDao.getClassById(id);
}

int ClassService::addClass(const GymClass& gymClass) {
    // Walidacja danych zajêæ
    if (gymClass.getName().empty()) {
        throw std::invalid_argument("Nazwa zajêæ nie mo¿e byæ pusta");
    }

    if (gymClass.getTrainer().empty()) {
        throw std::invalid_argument("Nazwa trenera nie mo¿e byæ pusta");
    }

    if (gymClass.getMaxParticipants() <= 0) {
        throw std::invalid_argument("Maksymalna liczba uczestników musi byæ dodatnia");
    }

    if (gymClass.getDate().empty() || gymClass.getTime().empty()) {
        throw std::invalid_argument("Data i czas zajêæ nie mog¹ byæ puste");
    }

    if (gymClass.getDuration() <= 0) {
        throw std::invalid_argument("Czas trwania zajêæ musi byæ dodatni");
    }

    return classDao.addClass(gymClass);
}

bool ClassService::updateClass(const GymClass& gymClass) {
    // Walidacja podobna jak przy dodawaniu
    if (gymClass.getName().empty()) {
        throw std::invalid_argument("Nazwa zajêæ nie mo¿e byæ pusta");
    }

    if (gymClass.getTrainer().empty()) {
        throw std::invalid_argument("Nazwa trenera nie mo¿e byæ pusta");
    }

    if (gymClass.getMaxParticipants() <= 0) {
        throw std::invalid_argument("Maksymalna liczba uczestników musi byæ dodatnia");
    }

    if (gymClass.getDate().empty() || gymClass.getTime().empty()) {
        throw std::invalid_argument("Data i czas zajêæ nie mog¹ byæ puste");
    }

    if (gymClass.getDuration() <= 0) {
        throw std::invalid_argument("Czas trwania zajêæ musi byæ dodatni");
    }

    return classDao.updateClass(gymClass);
}

bool ClassService::removeClass(int id) {
    return classDao.deleteClass(id);
}

std::vector<Reservation> ClassService::getAllReservations() {
    return classDao.getAllReservations();
}

std::vector<Reservation> ClassService::getReservationsByClientId(int clientId) {
    return classDao.getReservationsByClientId(clientId);
}

std::vector<Reservation> ClassService::getReservationsByClassId(int classId) {
    return classDao.getReservationsByClassId(classId);
}

std::unique_ptr<Reservation> ClassService::getReservationById(int id) {
    return classDao.getReservationById(id);
}

int ClassService::addReservation(const Reservation& reservation) {
    // Walidacja rezerwacji
    if (reservation.getClientId() <= 0) {
        throw std::invalid_argument("Nieprawid³owy identyfikator klienta");
    }

    if (reservation.getClassId() <= 0) {
        throw std::invalid_argument("Nieprawid³owy identyfikator zajêæ");
    }

    // Sprawdzenie, czy klient ma aktywny karnet
    if (!isClientEligibleForClass(reservation.getClientId(), reservation.getClassId())) {
        throw std::invalid_argument("Klient nie ma aktywnego karnetu");
    }

    // Sprawdzenie dostêpnoœci miejsc
    if (getAvailableSpotsForClass(reservation.getClassId()) <= 0) {
        throw std::invalid_argument("Brak dostêpnych miejsc na zajêciach");
    }

    return classDao.addReservation(reservation);
}

bool ClassService::updateReservation(const Reservation& reservation) {
    return classDao.updateReservation(reservation);
}

bool ClassService::removeReservation(int id) {
    return classDao.deleteReservation(id);
}

bool ClassService::cancelReservation(int id) {
    auto reservation = classDao.getReservationById(id);

    if (!reservation) {
        return false;
    }

    reservation->setStatus("cancelled");
    return classDao.updateReservation(*reservation);
}

int ClassService::getAvailableSpotsForClass(int classId) {
    auto gymClass = classDao.getClassById(classId);

    if (!gymClass) {
        return 0;
    }

    int maxParticipants = gymClass->getMaxParticipants();
    int currentReservations = classDao.getReservationCountForClass(classId);

    return maxParticipants - currentReservations;
}

bool ClassService::isClientEligibleForClass(int clientId, int classId) {
    // Sprawdzenie, czy klient ma aktywny karnet
    auto membership = membershipService.getActiveMembershipForClient(clientId);

    if (!membership || !membership->isValid()) {
        return false;
    }

    // Tutaj mo¿na dodaæ dodatkowe warunki, np. czy klient nie ma ju¿ rezerwacji na te zajêcia

    return true;
}