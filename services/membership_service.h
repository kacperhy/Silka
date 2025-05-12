// services/membership_service.h
#ifndef MEMBERSHIP_SERVICE_H
#define MEMBERSHIP_SERVICE_H

#include <vector>
#include <memory>
#include <string>
#include "../models/membership.h"
#include "../database/dao/membership_dao.h"

class MembershipService {
public:
    explicit MembershipService(MembershipDAO& membershipDao);

    // Metody biznesowe
    std::vector<Membership> getAllMemberships();
    std::vector<Membership> getMembershipsByClientId(int clientId);
    std::unique_ptr<Membership> getMembershipById(int id);
    std::unique_ptr<Membership> getActiveMembershipForClient(int clientId);
    int addMembership(const Membership& membership);
    bool updateMembership(const Membership& membership);
    bool removeMembership(int id);
    bool renewMembership(int id, int days);

    // Generowanie karnetów standardowych
    Membership createMonthlyMembership(int clientId, bool isStudent = false);
    Membership createQuarterlyMembership(int clientId, bool isStudent = false);
    Membership createYearlyMembership(int clientId, bool isStudent = false);

private:
    MembershipDAO& membershipDao;

    // Prywatne metody pomocnicze
    double calculatePrice(const std::string& type, bool isStudent);
};

#endif // MEMBERSHIP_SERVICE_H