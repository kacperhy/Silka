// models/gym_class.h
#ifndef GYM_CLASS_H
#define GYM_CLASS_H

#include <string>
#include <vector>

class GymClass {
public:
    GymClass();
    GymClass(int id, const std::string& name, const std::string& trainer,
        int maxParticipants, const std::string& date, const std::string& time,
        int duration, const std::string& description);

    // Gettery
    int getId() const;
    std::string getName() const;
    std::string getTrainer() const;
    int getMaxParticipants() const;
    std::string getDate() const;
    std::string getTime() const;
    int getDuration() const;
    std::string getDescription() const;

    // Settery
    void setId(int id);
    void setName(const std::string& name);
    void setTrainer(const std::string& trainer);
    void setMaxParticipants(int maxParticipants);
    void setDate(const std::string& date);
    void setTime(const std::string& time);
    void setDuration(int duration);
    void setDescription(const std::string& description);

private:
    int id;
    std::string name;
    std::string trainer;
    int maxParticipants;
    std::string date;
    std::string time;
    int duration;  // czas trwania w minutach
    std::string description;
};

#endif // GYM_CLASS_H