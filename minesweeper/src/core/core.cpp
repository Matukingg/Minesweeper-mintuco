#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "core.hpp"

Core& Core::instance(){
    static Core c;
    return c;
}

bool Core::load_settings(const std::string& filename){
    std::ifstream in(filename);
    if(!in.is_open()) {
        std::cerr << "Error: Could not open settings file: " << filename << std::endl;
        return false;
    }
    std::string line;
    while(std::getline(in, line)) {
        if(line.empty()||line[0]== '#') continue;
        auto eq=line.find('=');
        if(eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);
        gameSettings[key] = value;
    }
    return true;
}
bool Core::save_settings(const std::string& filename) const{
    std::ofstream out(filename);
    if(!out.is_open()) {
        std::cerr << "Error: Could not open settings file for writing: " << filename << std::endl;
        return false;
    }
    for(const auto& pair : gameSettings) {
        out << pair.first << "=" << pair.second << "\n";
    }
    return true;
}

std::string Core::get_string(const std::string& key, const std::string& def) const{
    auto it = gameSettings.find(key);
    return (it != gameSettings.end()) ? it->second : def;
}
int Core::get_int (const std::string& key, int def) const{
    auto it= get_string(key);
    if(it.empty()) return def;
    try { return std::stoi(it); }
    catch (...) { return def; }
}
bool Core::get_bool (const std::string& key, bool def) const{
    auto it= get_string(key);
    if(it=="1" || it=="true") return true;
    if(it=="0" || it=="false") return false;
    return def;
}

void Core::set_string(const std::string& key, const std::string& value){
    gameSettings[key] = value;
}
void Core::set_int   (const std::string& key, int value){
    gameSettings[key] = std::to_string(value);
}
void Core::set_bool  (const std::string& key, bool value){
    gameSettings[key] = value ? "1" : "0";
}
