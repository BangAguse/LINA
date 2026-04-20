#ifndef NETWORK_COLLECTOR_H
#define NETWORK_COLLECTOR_H

#include <string>
#include <vector>
#include <map>
#include <json/json.h>
#include <thread>
#include <atomic>
#include <mutex>

// WiFi Network Structure
struct WiFiNetwork {
    std::string ssid;
    std::string bssid;
    int signal_strength;  // dBm (-100 to -30)
    int frequency;        // MHz (2400 or 5000)
    std::string security; // WPA2, WPA3, Open, etc
    std::string ip_range;
};

// Device Structure
struct Device {
    std::string mac_address;
    std::string ip_address;
    std::string hostname;
    std::string device_type;  // smartphone, laptop, iot, tablet, other
    int signal_strength;      // dBm
    long last_seen;           // timestamp
};

// Collection Result
struct CollectionResult {
    long timestamp;
    std::vector<WiFiNetwork> networks;
    std::vector<Device> devices;
    std::string error;
    bool success;
};

class NetworkCollector {
private:
    std::atomic<bool> is_collecting;
    std::thread collection_thread;
    std::vector<std::string> collected_events;
    std::mutex events_mutex;
    
    // Platform-specific methods
    CollectionResult collect_wifi_networks();
    CollectionResult collect_nearby_devices();
    void detect_device_type(Device& device);
    
public:
    NetworkCollector();
    ~NetworkCollector();
    
    // Start/stop collection
    bool start_collection();
    void stop_collection();
    bool is_active() const;
    
    // Get results
    CollectionResult get_latest_data();
    std::vector<std::string> get_events() const;

    // Thread-safe pop of accumulated events (clears internal buffer)
    std::vector<std::string> pop_events();
    
    // Convert to JSON
    Json::Value to_json();
    
    // Utilities
    std::string format_timestamp(long timestamp);
};

#endif // NETWORK_COLLECTOR_H
