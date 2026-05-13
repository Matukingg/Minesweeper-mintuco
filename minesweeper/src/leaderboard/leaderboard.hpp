#pragma once
#include <string>
#include <vector>

struct LeaderboardEntry {
    std::string name;
    int difficulty;  // 0=easy 1=medium 2=hard
    int elapsed_ms;
};

class Leaderboard {
    std::vector<LeaderboardEntry> entries;
public:
    void add(const LeaderboardEntry& e);
    bool load(const std::string& filename = "leaderboard.csv");
    bool save(const std::string& filename = "leaderboard.csv") const;

    // Sorted best-first for a given difficulty
    std::vector<LeaderboardEntry> get_sorted(int difficulty) const;

    // All entries for a difficulty in insertion order (for graphs)
    std::vector<LeaderboardEntry> get_all(int difficulty) const;

    // One player's entries for a difficulty in insertion order
    std::vector<LeaderboardEntry> get_player_history(const std::string& name,
                                                      int difficulty) const;

    // Unique player names in order of first appearance
    std::vector<std::string> get_players() const;

    bool empty() const { return entries.empty(); }
};
