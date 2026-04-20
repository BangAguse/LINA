#include "language_manager.h"
#include <fstream>
#include <iostream>
#include <algorithm>

LanguageManager::LanguageManager(const std::string& path) : current_language("en_US"), languages_path(path) {
    // Initialize with English
    set_language("en_US");
}

bool LanguageManager::load_language_file(const std::string& locale) {
    std::string file_path = languages_path + locale + ".json";
    
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to load language file: " << file_path << std::endl;
        return false;
    }
    
    Json::Value root;
    try {
        file >> root;
        file.close();
        
        language_cache[locale] = root;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing language file: " << e.what() << std::endl;
        return false;
    }
}

bool LanguageManager::set_language(const std::string& locale) {
    // Check if language is cached
    if (language_cache.find(locale) == language_cache.end()) {
        // Try to load it
        if (!load_language_file(locale)) {
            std::cerr << "Language not available: " << locale << std::endl;
            return false;
        }
    }
    
    current_language = locale;
    current_strings = language_cache[locale];
    return true;
}

std::string LanguageManager::get_current_language() const {
    return current_language;
}

std::vector<std::string> LanguageManager::get_available_languages() const {
    return {"id_ID", "en_US", "ar_SA"};
}

std::string LanguageManager::get(const std::string& key) const {
    if (current_strings.isMember("strings") && 
        current_strings["strings"].isMember(key)) {
        return current_strings["strings"][key].asString();
    }
    
    // Return key itself if not found
    return key;
}

bool LanguageManager::is_rtl() const {
    if (current_strings.isMember("direction")) {
        return current_strings["direction"].asString() == "rtl";
    }
    return false;
}

std::string LanguageManager::get_direction() const {
    if (current_strings.isMember("direction")) {
        std::string dir = current_strings["direction"].asString();
        return (dir == "rtl") ? "rtl" : "ltr";
    }
    return "ltr";
}

const Json::Value& LanguageManager::get_all_strings() const {
    return current_strings;
}
