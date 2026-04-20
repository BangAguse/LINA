#ifndef DATA_NORMALIZER_H
#define DATA_NORMALIZER_H

#include <string>
#include <vector>
#include <json/json.h>
#include "network_collector.h"

class DataNormalizer {
private:
    // Helper methods
    std::string sanitize_string(const std::string& input);
    std::string normalize_mac_address(const std::string& mac);
    std::string normalize_ip_address(const std::string& ip);
    bool validate_mac_format(const std::string& mac);
    bool validate_ip_format(const std::string& ip);
    int normalize_signal_strength(int signal);
    
public:
    DataNormalizer();
    
    // Normalize collection result
    CollectionResult normalize(const CollectionResult& raw_data);
    
    // Normalize JSON data
    Json::Value normalize_json(const Json::Value& raw_json);
    
    // Validation methods
    bool validate_network(const WiFiNetwork& network);
    bool validate_device(const Device& device);
    
    // Duplicate removal
    std::vector<WiFiNetwork> remove_duplicate_networks(const std::vector<WiFiNetwork>& networks);
    std::vector<Device> remove_duplicate_devices(const std::vector<Device>& devices);
};

#endif // DATA_NORMALIZER_H
