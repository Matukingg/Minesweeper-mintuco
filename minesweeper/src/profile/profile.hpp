#pragma once
#include <string>
#include <vector>

class ProfileManager {
    std::vector<std::string> profiles;
    std::string active;
public:
    void load(const std::string& filename = "profiles.txt");
    void save(const std::string& filename = "profiles.txt") const;

    void add(const std::string& name);
    void remove(int idx);           // clears active if it was that profile
    void set_active(const std::string& name);
    void clear_active();

    const std::string&              get_active() const { return active; }
    bool                            has_active() const { return !active.empty(); }
    const std::vector<std::string>& get_all()    const { return profiles; }
    bool exists(const std::string& name)         const;
};
