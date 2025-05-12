// database/dao/membership_dao.h
#ifndef MEMBERSHIP_DAO_H
#define MEMBERSHIP_DAO_H

#include <vector>
#include <memory>
#include <string>
#include "../../models/membership.h"
#include "../db_manager.h"

class MembershipDAO {
public:
    explicit MembershipDAO(DBManager& dbManager);

    // Metody dostêpu do danych
    std::vector<Membership> getAllMemberships();
    std::vector<Membership> getMembershipsByClientId(int clientId);
    std::unique_ptr<Membership> getMembershipById(int id);
    std::unique_ptr<Membership> getActiveMembershipForClient(int clientId);
    int addMembership(const Membership& membership);
    bool updateMembership(const Membership& membership);
    bool deleteMembership(int id);

private:
    DBManager& dbManager;

    // Metoda pomocnicza do konwersji wiersza rezultatu zapytania na obiekt karnetu
    Membership rowToMembership(sqlite3_stmt* stmt);
};

#endif // MEMBERSHIP_DAO_H