#include "metro.h"
#include <thread>
#include <iostream>
#include <chrono>
#include <random>

Train::Train(Branch b) : current_branch(b) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    id = dis(gen);
    direction = false;
}

int Train::get_id() const { return id; }
bool Train::get_direction() const { return direction; }
Branch Train::get_branch() const { return current_branch; }
void Train::set_branch(Branch b) { current_branch = b; }
void Train::change_direction() { direction = !direction; }

void Train::arrive_at_station(const Station& station, Clock& clock, std::ofstream& log_file) {
    station.lock_mutex(direction);
    std::string time = clock.get_time();
    std::string branch_color = station.get_branch() == Branch::RED ? "red" :
                              station.get_branch() == Branch::GREEN ? "green" :
                              station.get_branch() == Branch::PURPLE ? "purple" : "lime";
    log_file << "<p>Train <span style='color:blue'>" << id
             << "</span> arrived at station <span style='color:" << branch_color << "'>" << station.get_name()
             << "</span><p>\n" << std::flush;
    //std::cout << "Logged arrival for train " << id << " at " << station.get_name() << " at " << time << std::endl;

    clock.add_seconds(station.get_wait_time());
    std::this_thread::sleep_for(std::chrono::milliseconds(station.get_wait_time()));

    std::string next_station = station.get_next_station(direction);
    if (!next_station.empty()) {
        //time = clock.get_time();
        log_file << "<p>Train <span style='color:blue'>" << id
                 << "</span> departing from <span style='color:" << branch_color << "'>" << station.get_name()
                 << "</span> to <span style='color:" << branch_color << "'>" << next_station << "</span><p>\n" << std::flush;
        //std::cout << "Logged departure for train " << id << " from " << station.get_name() << " to " << next_station << " at " << time << std::endl;
    }
    station.unlock_mutex(direction);
}
