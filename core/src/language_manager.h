#ifndef LANGUAGE_MANAGER_H
#define LANGUAGE_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <json/json.h>

class LanguageManager {
private:
    std::string current_language;
    Json::Value current_strings;
    std::map<std::string, Json::Value> language_cache;
    std::string languages_path;
    
    // Load language file
    bool load_language_file(const std::string& locale);
    
public:
    LanguageManager(const std::string& path = "core/languages/");
    
    // Language management
    bool set_language(const std::string& locale); // id_ID, en_US, ar_SA
    std::string get_current_language() const;
    std::vector<std::string> get_available_languages() const;
    
    // Get translated strings
    std::string get(const std::string& key) const;
    
    // Check if RTL language
    bool is_rtl() const;
    std::string get_direction() const; // "ltr" or "rtl"
    
    // Get all strings for language
    const Json::Value& get_all_strings() const;
};

#endif // LANGUAGE_MANAGER_H
