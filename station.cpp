#include "metro.h"
#include <random>

Station::Station(const std::string& n, Branch b, bool depot, bool fork, bool last,
                const std::vector<std::string>& next, const std::vector<std::string>& prev,
                int wt, const std::vector<double>& fd, const std::pair<std::string, std::string>& tr)
    : name(n), branch(b), is_depot(depot), is_fork(fork), is_last(last), next_stations(next),
      prev_stations(prev), wait_time(wt), fork_distribution(fd), transition(tr) {}

std::string Station::get_name() const { return name; }
Branch Station::get_branch() const { return branch; }
bool Station::is_depot_station() const { return is_depot; }
bool Station::is_fork_station() const { return is_fork; }
bool Station::is_last_station() const { return is_last; }
int Station::get_wait_time() const { return wait_time; }
std::pair<std::string, std::string> Station::get_transition() const { return transition; }

std::string Station::get_next_station(bool direction) const {
    // Handle fork stations
    if (is_fork && !fork_distribution.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::discrete_distribution<> d(fork_distribution.begin(), fork_distribution.end());
        if (!direction && !next_stations.empty() && next_stations.size() > 1) { // Forward fork
            auto index = d(gen);
            if (index < next_stations.size()) {
                return next_stations[index];
            }
            return next_stations[0];
        } else if (direction && !prev_stations.empty() && prev_stations.size() > 1) { // Backward fork
            auto index = d(gen);
            if (index < prev_stations.size()) {
                return prev_stations[index];
            }
            return prev_stations[0];
        }
    }

    // Handle transition to another branch
    auto [trans_branch, trans_station] = transition;
    if (!trans_branch.empty() && !trans_station.empty()) {
        if (!direction && !next_stations.empty() && next_stations[0] == trans_station) {
            return trans_station; // e.g., 28 May to Nizami (forward)
        }
        if (direction && !prev_stations.empty() && prev_stations[0] == trans_station) {
            return trans_station; // e.g., Nizami to 28 May (backward)
        }
    }

    // Default: return the first station in the appropriate direction
    if (direction && !prev_stations.empty()) {
        return prev_stations[0];
    }
    if (!direction && !next_stations.empty()) {
        return next_stations[0];
    }
    return "";
}

void Station::lock_mutex(bool direction) const {
    if (direction) mutex_backward.lock();
    else mutex_forward.lock();
}

void Station::unlock_mutex(bool direction) const {
    if (direction) mutex_backward.unlock();
    else mutex_forward.unlock();
}