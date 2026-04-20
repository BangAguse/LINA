#include "ai_engine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

// Note: Full llama.cpp integration would require linking against llama.cpp library
// For now, this is a framework that can be integrated with llama.cpp

AIEngine::AIEngine(const std::string& model_file_path) 
    : model_path(model_file_path), 
      llama_ctx(nullptr), 
      is_initialized(false),
      max_tokens(500),
      temperature(0.3f),
      top_p(0.9f) {
}

AIEngine::~AIEngine() {
    shutdown();
}

bool AIEngine::initialize() {
    // Check if model file exists
    if (!model_exists()) {
        std::cerr << "Model file not found: " << model_path << std::endl;
        return false;
    }
    
    // Here we would initialize llama.cpp context
    // This requires linking with llama.cpp library
    // Example (pseudo-code):
    // llama_context_params params = llama_context_default_params();
    // params.n_ctx = 2048;
    // llama_ctx = llama_new_context_with_model(model, params);
    
    std::cout << "AI Engine initialized with model: " << model_path << std::endl;
    // Mark as initialized. In the future, this will hold a real llama_ctx pointer
    is_initialized = true;
    llama_ctx = reinterpret_cast<void*>(0x1); // dummy non-null marker for placeholder
    return true;
}

void AIEngine::shutdown() {
    if (llama_ctx != nullptr) {
        // llama_free(llama_ctx);
        llama_ctx = nullptr;
    }
    is_initialized = false;
}

bool AIEngine::is_ready() const {
    // Consider engine ready if initialized. When llama.cpp is integrated,
    // llama_ctx should be a valid context pointer.
    return is_initialized;
}

std::string AIEngine::build_system_prompt(const std::string& collected_data) {
    std::string system_prompt = R"(
Anda adalah LINA, asisten AI jaringan yang defensive dan read-only.

PERAN ANDA:
- Menjawab pertanyaan tentang jaringan LOKAL HANYA berdasarkan data yang disediakan
- TIDAK PERNAH membuat asumsi di luar data yang diberikan
- Jika data TIDAK CUKUP, WAJIB katakan "Data tidak tersedia untuk menjawab pertanyaan ini"
- Fokus pada KEAMANAN dan PRIVASI
- TIDAK PERNAH memberikan saran untuk exploit, attack, atau illegal activities
- Gunakan bahasa yang PROFESSIONAL dan EDUCATIONAL
- SELALU referensi data spesifik saat menjawab

DATA JARINGAN LOKAL SAAT INI:
)";
    
    system_prompt += collected_data;
    
    system_prompt += R"(

ATURAN KETAT:
1. Hanya jawab berdasarkan data di atas
2. Jangan spekulasi atau buat asumsi
3. Jangan rekomendasikan tools hacking atau exploits
4. Jangan bantu dengan aktivitas illegal
5. Fokus pada pembelajaran dan pemahaman
6. Selalu ethical dan professional

Siap menerima pertanyaan.
)";
    
    return system_prompt;
}

std::string AIEngine::query(const std::string& question, const Json::Value& network_data) {
    if (!is_ready()) {
        return "AI Engine tidak siap. Silakan initialize terlebih dahulu.";
    }
    
    // Convert network data to string
    Json::StreamWriterBuilder writer;
    std::string data_str = Json::writeString(writer, network_data);
    
    // Build system prompt with data
    std::string system_prompt = build_system_prompt(data_str);
    
    // Build the full prompt
    std::string full_prompt = system_prompt + "\n\nPertanyaan User: " + question;
    
    // Here we would call llama.cpp to generate response
    // Example (pseudo-code):
    // llama_eval(...);
    // std::string response = llama_generate_completion(...);
    
    // For now, return a placeholder that demonstrates the structure
    std::string response = "Berdasarkan data jaringan yang dikumpulkan:\n";
    
    // Parse network data and provide analysis
    if (network_data.isMember("networks")) {
        response += "- Terdeteksi " + std::to_string(network_data["networks"].size()) + " jaringan WiFi\n";
    }
    if (network_data.isMember("devices")) {
        response += "- Terdeteksi " + std::to_string(network_data["devices"].size()) + " perangkat di sekitar\n";
    }
    
    response += "\n[Awaiting full llama.cpp integration for complete AI responses]";
    
    return sanitize_output(response);
}

std::string AIEngine::sanitize_output(const std::string& raw_output) {
    std::string result = raw_output;
    
    // Remove any potential prompt injection attempts
    std::vector<std::string> dangerous_patterns = {
        "IGNORE",
        "FORGET",
        "OVERRIDE",
        "SYSTEM PROMPT",
        "INSTRUCTIONS"
    };
    
    for (const auto& pattern : dangerous_patterns) {
        size_t pos = 0;
        while ((pos = result.find(pattern, pos)) != std::string::npos) {
            result.erase(pos, pattern.length());
        }
    }
    
    // Limit output length
    if (result.length() > 2000) {
        result = result.substr(0, 2000) + "... [output truncated]";
    }
    
    return result;
}

void AIEngine::set_max_tokens(int tokens) {
    max_tokens = std::max(100, std::min(tokens, 2048));
}

void AIEngine::set_temperature(float temp) {
    temperature = std::max(0.0f, std::min(temp, 2.0f));
}

void AIEngine::set_top_p(float p) {
    top_p = std::max(0.0f, std::min(p, 1.0f));
}

std::string AIEngine::get_model_path() const {
    return model_path;
}

bool AIEngine::model_exists() const {
    std::ifstream file(model_path);
    return file.good();
}
