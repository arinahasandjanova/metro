#ifndef METRO_H
#define METRO_H

#include <string>
#include <vector>
#include <mutex>
#include <random>
#include <fstream>
#include <memory>
#include "json.hpp"

using json = nlohmann::json;

enum class Branch { RED, GREEN, PURPLE, LIME };

class Station {
private:
    std::string name;
    Branch branch;
    bool is_depot;
    bool is_fork;
    bool is_last;
    std::vector<std::string> next_stations;
    std::vector<std::string> prev_stations;
    int wait_time;
    mutable std::mutex mutex_forward;
    mutable std::mutex mutex_backward;
    std::vector<double> fork_distribution;
    std::pair<std::string, std::string> transition;

public:
    Station(const std::string& n, Branch b, bool depot, bool fork, bool last,
            const std::vector<std::string>& next, const std::vector<std::string>& prev,
            int wt, const std::vector<double>& fd = {}, const std::pair<std::string, std::string>& tr = {});
    std::string get_name() const;
    Branch get_branch() const;
    bool is_depot_station() const;
    bool is_fork_station() const;
    bool is_last_station() const;
    std::string get_next_station(bool direction) const;
    int get_wait_time() const;
    void lock_mutex(bool direction) const;
    void unlock_mutex(bool direction) const;
    std::pair<std::string, std::string> get_transition() const;
};

class Train {
private:
    int id;
    bool direction; // false: forward, true: backward
    Branch current_branch;

public:
    Train(Branch b);
    int get_id() const;
    bool get_direction() const;
    void change_direction();
    Branch get_branch() const;
    void set_branch(Branch b);
    void arrive_at_station(const Station& station, class Clock& clock, std::ofstream& log_file);
};

class Clock {
private:
    int seconds;

public:
    Clock();
    void tick();
    std::string get_time() const;
    bool is_after_midnight() const;
    void add_seconds(int sec);
};

void create_stations_from_json(const std::string& filename, std::map<std::string, std::vector<std::unique_ptr<Station>>>& stations);
void run_metro(const std::map<std::string, std::vector<std::unique_ptr<Station>>>& stations, std::vector<Train>& trains, Clock& clock);
void start_trains(int red_trains, int green_trains, int purple_trains, int lime_trains,
                 const std::map<std::string, std::vector<std::unique_ptr<Station>>>& stations, Clock& clock);

#endif