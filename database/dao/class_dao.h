// database/dao/class_dao.h
#ifndef CLASS_DAO_H
#define CLASS_DAO_H

#include <vector>
#include <memory>
#include <string>
#include "../../models/gym_class.h"
#include "../../models/reservation.h"
#include "../db_manager.h"

class ClassDAO {
public:
    explicit ClassDAO(DBManager& dbManager);

    // Metody dostêpu do danych dla zajêæ
    std::vector<GymClass> getAllClasses();
    std::vector<GymClass> getUpcomingClasses();
    std::unique_ptr<GymClass> getClassById(int id);
    int addClass(const GymClass& gymClass);
    bool updateClass(const GymClass& gymClass);
    bool deleteClass(int id);

    // Metody dostêpu do danych dla rezerwacji
    std::vector<Reservation> getAllReservations();
    std::vector<Reservation> getReservationsByClientId(int clientId);
    std::vector<Reservation> getReservationsByClassId(int classId);
    std::unique_ptr<Reservation> getReservationById(int id);
    int addReservation(const Reservation& reservation);
    bool updateReservation(const Reservation& reservation);
    bool deleteReservation(int id);
    int getReservationCountForClass(int classId);

private:
    DBManager& dbManager;

    // Metody pomocnicze do konwersji wierszy na obiekty
    GymClass rowToGymClass(sqlite3_stmt* stmt);
    Reservation rowToReservation(sqlite3_stmt* stmt);
};

#endif // CLASS_DAO_H