// services/class_service.h
#ifndef CLASS_SERVICE_H
#define CLASS_SERVICE_H

#include <vector>
#include <memory>
#include <string>
#include "../models/gym_class.h"
#include "../models/reservation.h"
#include "../database/dao/class_dao.h"
#include "membership_service.h"

class ClassService {
public:
    ClassService(ClassDAO& classDao, MembershipService& membershipService);

    // Metody biznesowe dla zajêæ
    std::vector<GymClass> getAllClasses();
    std::vector<GymClass> getUpcomingClasses();
    std::unique_ptr<GymClass> getClassById(int id);
    int addClass(const GymClass& gymClass);
    bool updateClass(const GymClass& gymClass);
    bool removeClass(int id);

    // Metody biznesowe dla rezerwacji
    std::vector<Reservation> getAllReservations();
    std::vector<Reservation> getReservationsByClientId(int clientId);
    std::vector<Reservation> getReservationsByClassId(int classId);
    std::unique_ptr<Reservation> getReservationById(int id);
    int addReservation(const Reservation& reservation);
    bool updateReservation(const Reservation& reservation);
    bool removeReservation(int id);
    bool cancelReservation(int id);

    // Metody pomocnicze
    int getAvailableSpotsForClass(int classId);
    bool isClientEligibleForClass(int clientId, int classId);

private:
    ClassDAO& classDao;
    MembershipService& membershipService;
};

#endif // CLASS_SERVICE_H