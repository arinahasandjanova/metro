#include "metro.h"
#include <fstream>

void create_stations_from_json(const std::string& filename, std::map<std::string, std::vector<std::unique_ptr<Station>>>& stations) {
    std::ifstream file(filename);
    json j;
    file >> j;

    for (auto& [branch_name, branch_stations] : j["branches"].items()) {
        Branch branch = branch_name == "red" ? Branch::RED :
                        branch_name == "green" ? Branch::GREEN :
                        branch_name == "purple" ? Branch::PURPLE : Branch::LIME;
        std::vector<std::unique_ptr<Station>> branch_stations_vec;
        for (const auto& s : branch_stations) {
            std::vector<std::string> next = s.contains("next_stations") ? s["next_stations"].get<std::vector<std::string>>() : std::vector<std::string>();
            std::vector<std::string> prev = s.contains("prev_stations") ? s["prev_stations"].get<std::vector<std::string>>() : std::vector<std::string>();
            std::vector<double> fork_dist = s.contains("fork_distribution") ? s["fork_distribution"].get<std::vector<double>>() : std::vector<double>();
            std::pair<std::string, std::string> transition = s.contains("transition_to") ?
                std::make_pair(s["transition_to"]["branch"].get<std::string>(), s["transition_to"]["station"].get<std::string>()) :
                std::make_pair("", "");
            branch_stations_vec.emplace_back(std::make_unique<Station>(
                s["name"].get<std::string>(),
                branch,
                s["is_depot"].get<bool>(),
                s["is_fork"].get<bool>(),
                s["is_last"].get<bool>(),
                next,
                prev,
                s["wait_time"].get<int>(),
                fork_dist,
                transition
            ));
        }
        stations[branch_name] = std::move(branch_stations_vec);
    }
}