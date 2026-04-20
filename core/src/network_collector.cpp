#include "network_collector.h"
#include <chrono>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>

#ifdef _WIN32
    #include <winsock2.h>
    #include <iphlpapi.h>
    #pragma comment(lib, "iphlpapi.lib")
    #pragma comment(lib, "ws2_32.lib")
#elif __APPLE__
    #include <ifaddrs.h>
    #include <net/if.h>
#else // Linux
    #include <ifaddrs.h>
    #include <net/if.h>
    #include <sys/socket.h>
#endif

NetworkCollector::NetworkCollector() : is_collecting(false) {}

NetworkCollector::~NetworkCollector() {
    stop_collection();
}

bool NetworkCollector::start_collection() {
    if (is_collecting) return false;
    
    is_collecting = true;
    collection_thread = std::thread([this]() {
        while (is_collecting) {
            auto wifi = collect_wifi_networks();
            auto devices = collect_nearby_devices();
            
            if (wifi.success) {
                    std::string ev = "[" + format_timestamp(wifi.timestamp) + "] WiFi scan completed";
                    {
                        std::lock_guard<std::mutex> lock(events_mutex);
                        collected_events.push_back(ev);
                    }
            }
            if (devices.success) {
                    std::string ev = "[" + format_timestamp(devices.timestamp) + "] Device scan completed: " + std::to_string(devices.devices.size()) + " devices found";
                    {
                        std::lock_guard<std::mutex> lock(events_mutex);
                        collected_events.push_back(ev);
                    }
            }
            
            // Sleep for 10 seconds before next scan
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    });
    
    return true;
}

std::vector<std::string> NetworkCollector::pop_events() {
    std::vector<std::string> out;
    std::lock_guard<std::mutex> lock(events_mutex);
    out.swap(collected_events);
    return out;
}

void NetworkCollector::stop_collection() {
    is_collecting = false;
    if (collection_thread.joinable()) {
        collection_thread.join();
    }
}

bool NetworkCollector::is_active() const {
    return is_collecting;
}

CollectionResult NetworkCollector::collect_wifi_networks() {
    CollectionResult result;
    result.timestamp = std::chrono::system_clock::now().time_since_epoch().count() / 1000000000LL;
    result.success = false;
    
    #ifdef _WIN32
        // Windows implementation using netsh
        system("netsh wlan show network mode=bssid > wifi_scan.txt");
        std::ifstream file("wifi_scan.txt");
        std::string line;
        
        while (std::getline(file, line)) {
            if (line.find("SSID") != std::string::npos) {
                WiFiNetwork net;
                net.ssid = line.substr(line.find(":") + 2);
                net.signal_strength = -50; // placeholder
                net.frequency = 2400;
                net.security = "WPA2";
                result.networks.push_back(net);
            }
        }
        file.close();
        result.success = true;
    
    #elif __APPLE__
        // macOS implementation
        // Read from /Library/Preferences/SystemConfiguration/com.apple.wifi.message-tracer.plist
        // Simplified: return mock data for now
        WiFiNetwork net1;
        net1.ssid = "ExampleNetwork1";
        net1.bssid = "AA:BB:CC:DD:EE:FF";
        net1.signal_strength = -45;
        net1.frequency = 2400;
        net1.security = "WPA2";
        result.networks.push_back(net1);
        result.success = true;
    
    #else // Linux
        // Linux implementation using iwconfig or nmcli
        system("nmcli dev wifi list > wifi_scan.txt 2>/dev/null || iw dev wlan0 scan > wifi_scan.txt 2>/dev/null");
        std::ifstream file("wifi_scan.txt");
        std::string line;
        
        while (std::getline(file, line)) {
            // Parse nmcli output or iw output
            if (line.find("SSID") != std::string::npos || line.find("BSSID") != std::string::npos) {
                // Parse and add to networks
                WiFiNetwork net;
                net.signal_strength = -55;
                net.frequency = 2400;
                net.security = "WPA2";
                result.networks.push_back(net);
            }
        }
        file.close();
        result.success = true;
    #endif
    
    return result;
}

CollectionResult NetworkCollector::collect_nearby_devices() {
    CollectionResult result;
    result.timestamp = std::chrono::system_clock::now().time_since_epoch().count() / 1000000000LL;
    result.success = false;
    
    #ifdef _WIN32
        // Windows: use arp -a
        system("arp -a > arp_cache.txt");
        std::ifstream file("arp_cache.txt");
        std::string line;
        
        while (std::getline(file, line)) {
            // Parse ARP output
            if (line.find("-") != std::string::npos) {
                Device dev;
                dev.mac_address = "00:00:00:00:00:00"; // placeholder
                dev.ip_address = "192.168.1.1";
                dev.hostname = "device";
                dev.device_type = "unknown";
                dev.signal_strength = -60;
                dev.last_seen = result.timestamp;
                result.devices.push_back(dev);
            }
        }
        file.close();
        result.success = true;
    
    #else // Linux/macOS
        // Use arp-scan or arp
        system("arp -a > arp_cache.txt 2>/dev/null || arp-scan -l > arp_cache.txt 2>/dev/null");
        std::ifstream file("arp_cache.txt");
        std::string line;
        
        while (std::getline(file, line)) {
            // Parse ARP output
            if (!line.empty() && line.find(":") != std::string::npos) {
                Device dev;
                dev.mac_address = "00:00:00:00:00:00";
                dev.ip_address = "192.168.1.1";
                dev.hostname = "device";
                dev.device_type = "unknown";
                dev.signal_strength = -65;
                dev.last_seen = result.timestamp;
                result.devices.push_back(dev);
            }
        }
        file.close();
        result.success = true;
    #endif
    
    return result;
}

void NetworkCollector::detect_device_type(Device& device) {
    // Simple device type detection based on MAC address OUI (Organizationally Unique Identifier)
    std::string mac_upper = device.mac_address;
    std::transform(mac_upper.begin(), mac_upper.end(), mac_upper.begin(), ::toupper);
    
    // Apple devices
    if (mac_upper.substr(0, 8) == "00:1A:2B" || mac_upper.substr(0, 8) == "A4:5E:60") {
        device.device_type = "smartphone";
    }
    // Dell/Lenovo laptop
    else if (mac_upper.substr(0, 8) == "00:0B:85" || mac_upper.substr(0, 8) == "00:13:72") {
        device.device_type = "laptop";
    }
    // IoT devices
    else if (mac_upper.substr(0, 8) == "00:04:A3") {
        device.device_type = "iot";
    }
    else {
        device.device_type = "other";
    }
}

CollectionResult NetworkCollector::get_latest_data() {
    auto wifi = collect_wifi_networks();
    auto devices = collect_nearby_devices();
    
    // Merge results
    CollectionResult merged;
    merged.timestamp = std::chrono::system_clock::now().time_since_epoch().count() / 1000000000LL;
    merged.networks = wifi.networks;
    merged.devices = devices.devices;
    
    // Detect device types
    for (auto& device : merged.devices) {
        detect_device_type(device);
    }
    
    merged.success = wifi.success && devices.success;
    return merged;
}

std::vector<std::string> NetworkCollector::get_events() const {
    return collected_events;
}

Json::Value NetworkCollector::to_json() {
    auto data = get_latest_data();
    Json::Value root;
    
    root["timestamp"] = (Json::Value::Int64)data.timestamp;
    root["success"] = data.success;
    
    // Networks
    Json::Value networks_json(Json::arrayValue);
    for (const auto& net : data.networks) {
        Json::Value net_obj;
        net_obj["ssid"] = net.ssid;
        net_obj["bssid"] = net.bssid;
        net_obj["signal"] = net.signal_strength;
        net_obj["frequency"] = net.frequency;
        net_obj["security"] = net.security;
        networks_json.append(net_obj);
    }
    root["networks"] = networks_json;
    
    // Devices
    Json::Value devices_json(Json::arrayValue);
    for (const auto& dev : data.devices) {
        Json::Value dev_obj;
        dev_obj["mac"] = dev.mac_address;
        dev_obj["ip"] = dev.ip_address;
        dev_obj["hostname"] = dev.hostname;
        dev_obj["type"] = dev.device_type;
        dev_obj["signal"] = dev.signal_strength;
        devices_json.append(dev_obj);
    }
    root["devices"] = devices_json;
    
    return root;
}

std::string NetworkCollector::format_timestamp(long timestamp) {
    auto time = std::chrono::system_clock::from_time_t(timestamp);
    auto tm = std::localtime(&timestamp);
    
    std::stringstream ss;
    ss << std::put_time(tm, "%H:%M:%S");
    return ss.str();
}
