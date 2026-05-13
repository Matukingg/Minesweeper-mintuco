#pragma once
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <iostream>
#include <unordered_map>

class Core {
private:
    std::unordered_map<std::string, std::string> gameSettings = {
        {"fullscreen", "false"},
        {"grid_width", "10"},
        {"grid_height", "10"},
        {"num_mines", "15"},
        {"difficulty", "normal"}
    };
public:
    Core() {
        if (!al_init()) {
            std::cerr << "Failed to initialize Allegro." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if (!al_init_primitives_addon()) {
            std::cerr << "Failed to initialize Allegro primitives addon." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if (!al_init_image_addon()) {
            std::cerr << "Failed to initialize Allegro image addon." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if (!al_install_mouse()) {
            std::cerr << "Failed to install mouse." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if (!al_init_font_addon()) {
            std::cerr << "Failed to initialize font addon." << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if (!al_install_keyboard()) {
            std::cerr << "Failed to install keyboard." << std::endl;
            std::exit(EXIT_FAILURE);
        }
    };

    ~Core() {
        al_uninstall_keyboard();
        al_shutdown_font_addon();
        al_uninstall_mouse();
        al_shutdown_image_addon();
        al_shutdown_primitives_addon();
        al_uninstall_system();
        std::cout << "Allegro cleaned up successfully." << std::endl;
    };

    static Core& instance();

    bool load_settings(const std::string& filename = "settings.txt");
    bool save_settings(const std::string& filename = "settings.txt") const;

    std::string get_string(const std::string& key, const std::string& def="") const;
    int get_int (const std::string& key, int def=0) const;
    bool get_bool (const std::string& key, bool def=false) const;

    void set_string(const std::string& key, const std::string& value);
    void set_int   (const std::string& key, int value);
    void set_bool  (const std::string& key, bool value);

};