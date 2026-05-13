#include "leaderboard.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

void Leaderboard::add(const LeaderboardEntry& e) {
    entries.push_back(e);
}

bool Leaderboard::load(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) return false;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        std::string name, diff_s, ms_s;
        if (!std::getline(ss, name,   ',')) continue;
        if (!std::getline(ss, diff_s, ',')) continue;
        if (!std::getline(ss, ms_s,   ',')) continue;
        try { entries.push_back({name, std::stoi(diff_s), std::stoi(ms_s)}); }
        catch (...) {}
    }
    return true;
}

bool Leaderboard::save(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) return false;
    for (const auto& e : entries)
        out << e.name << ',' << e.difficulty << ',' << e.elapsed_ms << '\n';
    return true;
}

std::vector<LeaderboardEntry> Leaderboard::get_sorted(int difficulty) const {
    std::vector<LeaderboardEntry> r;
    for (const auto& e : entries)
        if (e.difficulty == difficulty) r.push_back(e);
    std::sort(r.begin(), r.end(),
        [](const LeaderboardEntry& a, const LeaderboardEntry& b){
            return a.elapsed_ms < b.elapsed_ms; });
    return r;
}

std::vector<LeaderboardEntry> Leaderboard::get_all(int difficulty) const {
    std::vector<LeaderboardEntry> r;
    for (const auto& e : entries)
        if (e.difficulty == difficulty) r.push_back(e);
    return r;
}

std::vector<LeaderboardEntry> Leaderboard::get_player_history(
        const std::string& name, int difficulty) const {
    std::vector<LeaderboardEntry> r;
    for (const auto& e : entries)
        if (e.name == name && e.difficulty == difficulty) r.push_back(e);
    return r;
}

std::vector<std::string> Leaderboard::get_players() const {
    std::vector<std::string> names;
    for (const auto& e : entries) {
        if (std::find(names.begin(), names.end(), e.name) == names.end())
            names.push_back(e.name);
    }
    return names;
}
