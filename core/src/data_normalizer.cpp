#include "data_normalizer.h"
#include <algorithm>
#include <regex>
#include <sstream>
#include <iomanip>
#include <cctype>

DataNormalizer::DataNormalizer() {}

std::string DataNormalizer::sanitize_string(const std::string& input) {
    std::string result;
    
    for (char c : input) {
        // Allow alphanumeric, spaces, hyphens, underscores, dots
        if (std::isalnum(c) || c == ' ' || c == '-' || c == '_' || c == '.') {
            result += c;
        }
    }
    
    // Trim whitespace
    auto start = result.find_first_not_of(" \t\n\r");
    auto end = result.find_last_not_of(" \t\n\r");
    
    if (start == std::string::npos) return "";
    return result.substr(start, end - start + 1);
}

std::string DataNormalizer::normalize_mac_address(const std::string& mac) {
    std::string result = mac;
    
    // Convert to uppercase
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    
    // Remove colons and add them back in correct format
    std::string cleaned;
    for (char c : result) {
        if (std::isxdigit(c)) {
            cleaned += c;
        }
    }
    
    // Format as XX:XX:XX:XX:XX:XX
    if (cleaned.length() == 12) {
        std::string formatted;
        for (int i = 0; i < 12; i += 2) {
            if (i > 0) formatted += ":";
            formatted += cleaned.substr(i, 2);
        }
        return formatted;
    }
    
    return mac; // Return original if invalid
}

std::string DataNormalizer::normalize_ip_address(const std::string& ip) {
    // Basic validation: check for 4 octets separated by dots
    std::regex ip_regex("^([0-9]{1,3}\\.){3}[0-9]{1,3}$");
    
    if (std::regex_match(ip, ip_regex)) {
        // Split and validate each octet
        std::istringstream iss(ip);
        std::string octet;
        std::vector<int> octets;
        
        while (std::getline(iss, octet, '.')) {
            int val = std::stoi(octet);
            if (val > 255) return "0.0.0.0"; // Invalid
            octets.push_back(val);
        }
        
        if (octets.size() == 4) {
            return ip; // Valid IP
        }
    }
    
    return "0.0.0.0"; // Return invalid marker
}

bool DataNormalizer::validate_mac_format(const std::string& mac) {
    std::regex mac_regex("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$");
    return std::regex_match(mac, mac_regex);
}

bool DataNormalizer::validate_ip_format(const std::string& ip) {
    std::regex ip_regex("^([0-9]{1,3}\\.){3}[0-9]{1,3}$");
    return std::regex_match(ip, ip_regex);
}

int DataNormalizer::normalize_signal_strength(int signal) {
    // Signal strength should be between -100 and -30 dBm
    if (signal < -100) return -100;
    if (signal > -30) return -30;
    return signal;
}

CollectionResult DataNormalizer::normalize(const CollectionResult& raw_data) {
    CollectionResult result = raw_data;
    
    // Normalize networks
    for (auto& network : result.networks) {
        network.ssid = sanitize_string(network.ssid);
        network.bssid = normalize_mac_address(network.bssid);
        network.signal_strength = normalize_signal_strength(network.signal_strength);
        network.security = sanitize_string(network.security);
    }
    
    // Remove duplicates
    result.networks = remove_duplicate_networks(result.networks);
    
    // Normalize devices
    for (auto& device : result.devices) {
        device.mac_address = normalize_mac_address(device.mac_address);
        device.ip_address = normalize_ip_address(device.ip_address);
        device.hostname = sanitize_string(device.hostname);
        device.device_type = sanitize_string(device.device_type);
        device.signal_strength = normalize_signal_strength(device.signal_strength);
    }
    
    // Remove duplicates
    result.devices = remove_duplicate_devices(result.devices);
    
    return result;
}

Json::Value DataNormalizer::normalize_json(const Json::Value& raw_json) {
    Json::Value normalized;
    
    if (raw_json.isMember("timestamp")) {
        normalized["timestamp"] = raw_json["timestamp"];
    }
    
    // Normalize networks
    if (raw_json.isMember("networks") && raw_json["networks"].isArray()) {
        Json::Value networks_array(Json::arrayValue);
        
        for (const auto& net : raw_json["networks"]) {
            Json::Value normalized_net;
            
            if (net.isMember("ssid")) {
                normalized_net["ssid"] = sanitize_string(net["ssid"].asString());
            }
            if (net.isMember("bssid")) {
                normalized_net["bssid"] = normalize_mac_address(net["bssid"].asString());
            }
            if (net.isMember("signal")) {
                normalized_net["signal"] = normalize_signal_strength(net["signal"].asInt());
            }
            if (net.isMember("frequency")) {
                normalized_net["frequency"] = net["frequency"].asInt();
            }
            if (net.isMember("security")) {
                normalized_net["security"] = sanitize_string(net["security"].asString());
            }
            
            networks_array.append(normalized_net);
        }
        normalized["networks"] = networks_array;
    }
    
    // Normalize devices
    if (raw_json.isMember("devices") && raw_json["devices"].isArray()) {
        Json::Value devices_array(Json::arrayValue);
        
        for (const auto& dev : raw_json["devices"]) {
            Json::Value normalized_dev;
            
            if (dev.isMember("mac")) {
                normalized_dev["mac"] = normalize_mac_address(dev["mac"].asString());
            }
            if (dev.isMember("ip")) {
                normalized_dev["ip"] = normalize_ip_address(dev["ip"].asString());
            }
            if (dev.isMember("hostname")) {
                normalized_dev["hostname"] = sanitize_string(dev["hostname"].asString());
            }
            if (dev.isMember("type")) {
                normalized_dev["type"] = sanitize_string(dev["type"].asString());
            }
            if (dev.isMember("signal")) {
                normalized_dev["signal"] = normalize_signal_strength(dev["signal"].asInt());
            }
            
            devices_array.append(normalized_dev);
        }
        normalized["devices"] = devices_array;
    }
    
    return normalized;
}

bool DataNormalizer::validate_network(const WiFiNetwork& network) {
    // Check required fields
    if (network.ssid.empty() || network.bssid.empty()) {
        return false;
    }
    
    // Validate BSSID format
    if (!validate_mac_format(network.bssid)) {
        return false;
    }
    
    // Validate signal strength
    if (network.signal_strength < -100 || network.signal_strength > -30) {
        return false;
    }
    
    // Validate frequency
    if (network.frequency != 2400 && network.frequency != 5000 && network.frequency != 6000) {
        return false;
    }
    
    return true;
}

bool DataNormalizer::validate_device(const Device& device) {
    // Check required fields
    if (device.mac_address.empty()) {
        return false;
    }
    
    // Validate MAC address
    if (!validate_mac_format(device.mac_address)) {
        return false;
    }
    
    // Validate signal strength
    if (device.signal_strength < -100 || device.signal_strength > -30) {
        return false;
    }
    
    return true;
}

std::vector<WiFiNetwork> DataNormalizer::remove_duplicate_networks(const std::vector<WiFiNetwork>& networks) {
    std::vector<WiFiNetwork> result;
    std::vector<std::string> seen_bssids;
    
    for (const auto& network : networks) {
        // Check if we've already added this BSSID
        if (std::find(seen_bssids.begin(), seen_bssids.end(), network.bssid) == seen_bssids.end()) {
            result.push_back(network);
            seen_bssids.push_back(network.bssid);
        }
    }
    
    return result;
}

std::vector<Device> DataNormalizer::remove_duplicate_devices(const std::vector<Device>& devices) {
    std::vector<Device> result;
    std::vector<std::string> seen_macs;
    
    for (const auto& device : devices) {
        // Check if we've already added this MAC
        if (std::find(seen_macs.begin(), seen_macs.end(), device.mac_address) == seen_macs.end()) {
            result.push_back(device);
            seen_macs.push_back(device.mac_address);
        }
    }
    
    return result;
}
