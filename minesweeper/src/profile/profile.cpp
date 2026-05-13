#include "profile.hpp"
#include <fstream>
#include <algorithm>

void ProfileManager::load(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) return;
    std::string line;
    while (std::getline(in, line))
        if (!line.empty()) profiles.push_back(line);
}

void ProfileManager::save(const std::string& filename) const {
    std::ofstream out(filename);
    for (const auto& p : profiles) out << p << '\n';
}

void ProfileManager::add(const std::string& name) {
    if (!exists(name)) profiles.push_back(name);
}

void ProfileManager::remove(int idx) {
    if (idx < 0 || idx >= (int)profiles.size()) return;
    if (profiles[idx] == active) active = "";
    profiles.erase(profiles.begin() + idx);
}

void ProfileManager::set_active(const std::string& name) {
    active = name;
}

void ProfileManager::clear_active() {
    active = "";
}

bool ProfileManager::exists(const std::string& name) const {
    return std::find(profiles.begin(), profiles.end(), name) != profiles.end();
}
