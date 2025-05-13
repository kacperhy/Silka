// database/dao/class_dao.cpp
#include "class_dao.h"
#include <sqlite3.h>
#include <iostream>

ClassDAO::ClassDAO(DBManager& dbManager) : dbManager(dbManager) {
}

std::vector<GymClass> ClassDAO::getAllClasses() {
    std::vector<GymClass> classes;

    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, name, trainer, max_participants, date, time, duration, description FROM classes";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        classes.push_back(rowToGymClass(stmt));
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return classes;
}

std::vector<GymClass> ClassDAO::getUpcomingClasses() {
    std::vector<GymClass> classes;

    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, name, trainer, max_participants, date, time, duration, description "
        "FROM classes WHERE date(date) >= date('now') "
        "ORDER BY date ASC, time ASC";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        classes.push_back(rowToGymClass(stmt));
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return classes;
}

std::unique_ptr<GymClass> ClassDAO::getClassById(int id) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, name, trainer, max_participants, date, time, duration, description "
        "FROM classes WHERE id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        auto gymClass = std::make_unique<GymClass>(rowToGymClass(stmt));
        sqlite3_finalize(stmt);
        return gymClass;
    }
    else {
        sqlite3_finalize(stmt);
        return nullptr;
    }
}

int ClassDAO::addClass(const GymClass& gymClass) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "INSERT INTO classes (name, trainer, max_participants, date, time, duration, description) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, gymClass.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, gymClass.getTrainer().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, gymClass.getMaxParticipants());
    sqlite3_bind_text(stmt, 4, gymClass.getDate().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, gymClass.getTime().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, gymClass.getDuration());
    sqlite3_bind_text(stmt, 7, gymClass.getDescription().c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return dbManager.getLastInsertRowId();
}

bool ClassDAO::updateClass(const GymClass& gymClass) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "UPDATE classes SET name = ?, trainer = ?, max_participants = ?, "
        "date = ?, time = ?, duration = ?, description = ? WHERE id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, gymClass.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, gymClass.getTrainer().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, gymClass.getMaxParticipants());
    sqlite3_bind_text(stmt, 4, gymClass.getDate().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, gymClass.getTime().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, gymClass.getDuration());
    sqlite3_bind_text(stmt, 7, gymClass.getDescription().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 8, gymClass.getId());

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool ClassDAO::deleteClass(int id) {
    sqlite3* db = dbManager.getConnection();

    // Najpierw usuwamy powi¹zane rezerwacje
    const char* deleteReservationsQuery = "DELETE FROM reservations WHERE class_id = ?";

    sqlite3_stmt* stmt1;
    int rc = sqlite3_prepare_v2(db, deleteReservationsQuery, -1, &stmt1, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt1, 1, id);
    rc = sqlite3_step(stmt1);
    sqlite3_finalize(stmt1);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    // Teraz usuwamy same zajêcia
    const char* deleteClassQuery = "DELETE FROM classes WHERE id = ?";

    sqlite3_stmt* stmt2;
    rc = sqlite3_prepare_v2(db, deleteClassQuery, -1, &stmt2, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt2, 1, id);
    rc = sqlite3_step(stmt2);
    sqlite3_finalize(stmt2);

    return rc == SQLITE_DONE;
}

std::vector<Reservation> ClassDAO::getAllReservations() {
    std::vector<Reservation> reservations;

    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, client_id, class_id, reservation_date, status FROM reservations";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        reservations.push_back(rowToReservation(stmt));
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return reservations;
}

std::vector<Reservation> ClassDAO::getReservationsByClientId(int clientId) {
    std::vector<Reservation> reservations;

    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, client_id, class_id, reservation_date, status FROM reservations "
        "WHERE client_id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, clientId);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        reservations.push_back(rowToReservation(stmt));
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return reservations;
}

std::vector<Reservation> ClassDAO::getReservationsByClassId(int classId) {
    std::vector<Reservation> reservations;

    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, client_id, class_id, reservation_date, status FROM reservations "
        "WHERE class_id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, classId);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        reservations.push_back(rowToReservation(stmt));
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return reservations;
}

std::unique_ptr<Reservation> ClassDAO::getReservationById(int id) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT id, client_id, class_id, reservation_date, status FROM reservations "
        "WHERE id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        auto reservation = std::make_unique<Reservation>(rowToReservation(stmt));
        sqlite3_finalize(stmt);
        return reservation;
    }
    else {
        sqlite3_finalize(stmt);
        return nullptr;
    }
}

int ClassDAO::addReservation(const Reservation& reservation) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "INSERT INTO reservations (client_id, class_id, reservation_date, status) "
        "VALUES (?, ?, ?, ?)";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, reservation.getClientId());
    sqlite3_bind_int(stmt, 2, reservation.getClassId());
    sqlite3_bind_text(stmt, 3, reservation.getReservationDate().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, reservation.getStatus().c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw DatabaseException("B³¹d wykonania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    return dbManager.getLastInsertRowId();
}

bool ClassDAO::updateReservation(const Reservation& reservation) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "UPDATE reservations SET client_id = ?, class_id = ?, "
        "reservation_date = ?, status = ? WHERE id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, reservation.getClientId());
    sqlite3_bind_int(stmt, 2, reservation.getClassId());
    sqlite3_bind_text(stmt, 3, reservation.getReservationDate().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, reservation.getStatus().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, reservation.getId());

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool ClassDAO::deleteReservation(int id) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "DELETE FROM reservations WHERE id = ?";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

int ClassDAO::getReservationCountForClass(int classId) {
    sqlite3* db = dbManager.getConnection();
    const char* query = "SELECT COUNT(*) FROM reservations WHERE class_id = ? AND status = 'confirmed'";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        throw DatabaseException("B³¹d przygotowania zapytania: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, classId);

    rc = sqlite3_step(stmt);

    int count = 0;
    if (rc == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return count;
}

GymClass ClassDAO::rowToGymClass(sqlite3_stmt* stmt) {
    int id = sqlite3_column_int(stmt, 0);

    const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    const char* trainer = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    int maxParticipants = sqlite3_column_int(stmt, 3);
    const char* date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    const char* time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    int duration = sqlite3_column_int(stmt, 6);
    const char* description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));

    return GymClass(
        id,
        name ? std::string(name) : "",
        trainer ? std::string(trainer) : "",
        maxParticipants,
        date ? std::string(date) : "",
        time ? std::string(time) : "",
        duration,
        description ? std::string(description) : ""
    );
}

Reservation ClassDAO::rowToReservation(sqlite3_stmt* stmt) {
    int id = sqlite3_column_int(stmt, 0);
    int clientId = sqlite3_column_int(stmt, 1);
    int classId = sqlite3_column_int(stmt, 2);

    const char* reservationDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    const char* status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

    return Reservation(
        id,
        clientId,
        classId,
        reservationDate ? std::string(reservationDate) : "",
        status ? std::string(status) : "confirmed"
    );
}