// services/membership_service.cpp
#include "membership_service.h"
#include <stdexcept>

MembershipService::MembershipService(MembershipDAO& membershipDao) : membershipDao(membershipDao) {
}

std::vector<Membership> MembershipService::getAllMemberships() {
    return membershipDao.getAllMemberships();
}

std::vector<Membership> MembershipService::getMembershipsByClientId(int clientId) {
    return membershipDao.getMembershipsByClientId(clientId);
}

std::unique_ptr<Membership> MembershipService::getMembershipById(int id) {
    return membershipDao.getMembershipById(id);
}

std::unique_ptr<Membership> MembershipService::getActiveMembershipForClient(int clientId) {
    return membershipDao.getActiveMembershipForClient(clientId);
}

int MembershipService::addMembership(const Membership& membership) {
    // Walidacja danych karnetu
    if (membership.getClientId() <= 0) {
        throw std::invalid_argument("Nieprawid³owy identyfikator klienta");
    }

    if (membership.getPrice() < 0) {
        throw std::invalid_argument("Cena karnetu nie mo¿e byæ ujemna");
    }

    // Dodanie karnetu do bazy
    return membershipDao.addMembership(membership);
}

bool MembershipService::updateMembership(const Membership& membership) {
    // Walidacja podobna jak przy dodawaniu
    if (membership.getClientId() <= 0) {
        throw std::invalid_argument("Nieprawid³owy identyfikator klienta");
    }

    if (membership.getPrice() < 0) {
        throw std::invalid_argument("Cena karnetu nie mo¿e byæ ujemna");
    }

    return membershipDao.updateMembership(membership);
}

bool MembershipService::removeMembership(int id) {
    return membershipDao.deleteMembership(id);
}

bool MembershipService::renewMembership(int id, int days) {
    if (days <= 0) {
        throw std::invalid_argument("Liczba dni przed³u¿enia musi byæ dodatnia");
    }

    auto membership = membershipDao.getMembershipById(id);

    if (!membership) {
        return false;
    }

    // Przed³u¿enie karnetu
    std::string newEndDate;
    if (membership->isValid()) {
        // Jeœli karnet jest aktywny, przed³u¿amy od daty koñcowej
        newEndDate = Membership::addDaysToDate(membership->getEndDate(), days);
    }
    else {
        // Jeœli karnet wygas³, przed³u¿amy od dzisiaj
        newEndDate = Membership::addDaysToDate(Membership::getCurrentDate(), days);
        membership->setStartDate(Membership::getCurrentDate());
    }

    membership->setEndDate(newEndDate);
    membership->setIsActive(true);

    return membershipDao.updateMembership(*membership);
}

Membership MembershipService::createMonthlyMembership(int clientId, bool isStudent) {
    std::string currentDate = Membership::getCurrentDate();
    std::string endDate = Membership::addDaysToDate(currentDate, 30);

    std::string type = isStudent ? "student_monthly" : "normal_monthly";
    double price = calculatePrice(type, isStudent);

    return Membership(-1, clientId, type, currentDate, endDate, price, true);
}

Membership MembershipService::createQuarterlyMembership(int clientId, bool isStudent) {
    std::string currentDate = Membership::getCurrentDate();
    std::string endDate = Membership::addDaysToDate(currentDate, 90);

    std::string type = isStudent ? "student_quarterly" : "normal_quarterly";
    double price = calculatePrice(type, isStudent);

    return Membership(-1, clientId, type, currentDate, endDate, price, true);
}

Membership MembershipService::createYearlyMembership(int clientId, bool isStudent) {
    std::string currentDate = Membership::getCurrentDate();
    std::string endDate = Membership::addDaysToDate(currentDate, 365);

    std::string type = isStudent ? "student_yearly" : "normal_yearly";
    double price = calculatePrice(type, isStudent);

    return Membership(-1, clientId, type, currentDate, endDate, price, true);
}

double MembershipService::calculatePrice(const std::string& type, bool isStudent) {
    // Przyk³adowy cennik
    if (type == "normal_monthly" || type == "student_monthly") {
        return isStudent ? 89.0 : 119.0;
    }
    else if (type == "normal_quarterly" || type == "student_quarterly") {
        return isStudent ? 239.0 : 299.0;
    }
    else if (type == "normal_yearly" || type == "student_yearly") {
        return isStudent ? 799.0 : 999.0;
    }
    else {
        return 0.0;
    }
}