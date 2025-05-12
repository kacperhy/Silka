// models/gym_class.cpp
#include "gym_class.h"

GymClass::GymClass() : id(-1), name(""), trainer(""), maxParticipants(0),
date(""), time(""), duration(0), description("") {
}

GymClass::GymClass(int id, const std::string& name, const std::string& trainer,
    int maxParticipants, const std::string& date, const std::string& time,
    int duration, const std::string& description) :
    id(id), name(name), trainer(trainer), maxParticipants(maxParticipants),
    date(date), time(time), duration(duration), description(description) {
}

int GymClass::getId() const {
    return id;
}

std::string GymClass::getName() const {
    return name;
}

std::string GymClass::getTrainer() const {
    return trainer;
}

int GymClass::getMaxParticipants() const {
    return maxParticipants;
}

std::string GymClass::getDate() const {
    return date;
}

std::string GymClass::getTime() const {
    return time;
}

int GymClass::getDuration() const {
    return duration;
}

std::string GymClass::getDescription() const {
    return description;
}

void GymClass::setId(int id) {
    this->id = id;
}

void GymClass::setName(const std::string& name) {
    this->name = name;
}

void GymClass::setTrainer(const std::string& trainer) {
    this->trainer = trainer;
}

void GymClass::setMaxParticipants(int maxParticipants) {
    this->maxParticipants = maxParticipants;
}

void GymClass::setDate(const std::string& date) {
    this->date = date;
}

void GymClass::setTime(const std::string& time) {
    this->time = time;
}

void GymClass::setDuration(int duration) {
    this->duration = duration;
}

void GymClass::setDescription(const std::string& description) {
    this->description = description;
}