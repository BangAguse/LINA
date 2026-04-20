#ifndef AI_ENGINE_H
#define AI_ENGINE_H

#include <string>
#include <vector>
#include <json/json.h>

class AIEngine {
private:
    std::string model_path;
    void* llama_ctx;  // llama_context pointer
    bool is_initialized;
    int max_tokens;
    float temperature;
    float top_p;
    
    // Internal methods
    std::string build_system_prompt(const std::string& collected_data);
    std::string sanitize_output(const std::string& raw_output);
    
public:
    AIEngine(const std::string& model_file_path = "models/mistrallite.Q2_K.gguf");
    ~AIEngine();
    
    // Initialize/shutdown
    bool initialize();
    void shutdown();
    bool is_ready() const;
    
    // Query AI with network data
    std::string query(const std::string& question, const Json::Value& network_data);
    
    // Configuration
    void set_max_tokens(int tokens);
    void set_temperature(float temp);
    void set_top_p(float p);
    
    // Model info
    std::string get_model_path() const;
    bool model_exists() const;
};

#endif // AI_ENGINE_H
