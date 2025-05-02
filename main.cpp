#include "metro.h"
#include <iostream>
#include <thread>
#include <map>

void run_metro(const std::map<std::string, std::vector<std::unique_ptr<Station>>>& stations, std::vector<Train>& trains, Clock& clock) {
    std::map<std::string, std::ofstream> log_files;
    for (const auto& [branch_name, _] : stations) {
        log_files[branch_name].open("branch_" + branch_name + ".md");
        if (!log_files[branch_name].is_open()) {
            std::cerr << "Error: Failed to open branch_" << branch_name << ".md" << std::endl;
            return;
        }
        log_files[branch_name] << "<html><body>\n" << std::flush;
    }

    struct TrainState {
        Train& train;
        std::string current_station_name;
        std::string current_branch_name;
    };

    std::map<std::string, std::string> depot_stations = {
        {"red", "Bakmil"},
        {"green", "Darnaqul"},
        {"purple", "Khojasan"},
        {"lime", "Jafar Jabbarly"}
    };

    std::vector<TrainState> train_states;
    for (auto& train : trains) {
        std::string branch_name = train.get_branch() == Branch::RED ? "red" :
                                  train.get_branch() == Branch::GREEN ? "green" :
                                  train.get_branch() == Branch::PURPLE ? "purple" : "lime";
        train_states.push_back({train, depot_stations[branch_name], branch_name});
    }

    bool after_midnight = false;
    while (!(after_midnight && train_states.empty())) {
        std::vector<TrainState> remaining_trains;
        int train_offset = 0;

        if (clock.is_after_midnight() && !after_midnight) {
            after_midnight = true;
            std::cout << "Midnight reached, directing trains to depots\n";
        }

        for (auto& state : train_states) {
            auto& train = state.train;
            auto& current_station_name = state.current_station_name;
            auto& current_branch_name = state.current_branch_name;

            const auto& branch_stations = stations.at(current_branch_name);
            const Station* current_station = nullptr;
            for (const auto& station : branch_stations) {
                if (station->get_name() == current_station_name) {
                    current_station = station.get();
                    break;
                }
            }

            if (!current_station) {
                std::cerr << "Error: Station " << current_station_name << " not found on branch " << current_branch_name << " for train " << train.get_id() << std::endl;
                continue;
            }

            clock.add_seconds(train_offset * 30);
            train.arrive_at_station(*current_station, clock, log_files[current_branch_name]);
            log_files[current_branch_name] << std::flush;

            if (after_midnight && current_station->is_depot_station()) {
                std::cout << "Train " << train.get_id() << " reached depot " << current_station_name << ". Stopping.\n";
                continue;
            }

            std::string next_station = current_station->get_next_station(train.get_direction());

            if (next_station.empty()) {
                if (current_station->is_last_station() || current_station->is_depot_station()) {
                    train.change_direction();
                    next_station = current_station->get_next_station(train.get_direction());

                    if (next_station.empty()) {
                        std::cerr << "Error: No next station for train " << train.get_id() << " after changing direction at " << current_station_name << std::endl;
                        continue;
                    }
                } else {
                    std::cerr << "Error: Train " << train.get_id() << " stuck at " << current_station_name << std::endl;
                    continue;
                }
            }

            auto [trans_branch, trans_station] = current_station->get_transition();
            if (!trans_branch.empty() && next_station == trans_station) {
                std::string time = clock.get_time();
                log_files[current_branch_name] << "<p>Train <span style='color:blue'>"
                                               << train.get_id() << "</span> transitioning to <span style='color:" << trans_branch
                                               << "'>" << trans_station << "</span> on " << trans_branch << " branch<p>\n";
                current_branch_name = trans_branch;
                train.set_branch(
                    trans_branch == "red" ? Branch::RED :
                    trans_branch == "green" ? Branch::GREEN :
                    trans_branch == "purple" ? Branch::PURPLE : Branch::LIME
                );
            }

            current_station_name = next_station;
            remaining_trains.push_back({train, current_station_name, current_branch_name});
            train_offset++;
        }

        train_states = std::move(remaining_trains);

        if (train_states.empty() && !after_midnight) {
            std::cerr << "Warning: All trains removed before midnight, continuing simulation\n";
            clock.tick();
            continue;
        }

        if (!train_states.empty() || !after_midnight) {
            clock.tick();
        }
    }

    for (auto& [branch_name, file] : log_files) {
        file << "</body></html>\n";
        file.close();
    }
}

void start_trains(int red_trains, int green_trains, int purple_trains, int lime_trains,
                 const std::map<std::string, std::vector<std::unique_ptr<Station>>>& stations, Clock& clock) {
    std::vector<Train> trains;
    for (int i = 0; i < red_trains; ++i) trains.emplace_back(Branch::RED);
    for (int i = 0; i < green_trains; ++i) trains.emplace_back(Branch::GREEN);
    for (int i = 0; i < purple_trains; ++i) trains.emplace_back(Branch::PURPLE);
    for (int i = 0; i < lime_trains; ++i) trains.emplace_back(Branch::LIME);

    std::cout << "Starting " << trains.size() << " trains in single-threaded mode" << std::endl;
    run_metro(stations, trains, clock);
}

int main() {
    std::map<std::string, std::vector<std::unique_ptr<Station>>> stations;
    create_stations_from_json("stations.json", stations);

    if (stations.empty()) {
        std::cerr << "Error: No stations loaded from stations.json" << std::endl;
        return 1;
    }
    std::cout << "Loaded " << stations.size() << " branches from stations.json" << std::endl;

    int red_trains, green_trains, purple_trains, lime_trains;
    std::cout << "Enter number of trains for Red branch: ";
    std::cin >> red_trains;
    std::cout << "Enter number of trains for Green branch: ";
    std::cin >> green_trains;
    std::cout << "Enter number of trains for Purple branch: ";
    std::cin >> purple_trains;
    std::cout << "Enter number of trains for Lime branch: ";
    std::cin >> lime_trains;

    Clock clock;
    start_trains(red_trains, green_trains, purple_trains, lime_trains, stations, clock);

    std::cout << "Simulation completed" << std::endl;
    return 0;
}
