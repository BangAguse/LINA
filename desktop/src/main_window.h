#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QTimer>
#include <QProcess>
#include <memory>
#include <json/json.h>

// Include core headers instead of forward declarations
#include "network_collector.h"
#include "data_normalizer.h"
#include "ai_engine.h"
#include "language_manager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    // Core engines
    std::unique_ptr<NetworkCollector> network_collector;
    std::unique_ptr<DataNormalizer> data_normalizer;
    std::unique_ptr<AIEngine> ai_engine;
    std::unique_ptr<LanguageManager> language_manager;
    
    // UI Elements - Top Bar
    QLabel* app_title_label;
    QPushButton* settings_button;
    QComboBox* language_selector;
    
    // UI Elements - Left Panel
    QLabel* status_indicator;
    QPushButton* start_button;
    QPushButton* stop_button;
    QPushButton* refresh_button;
    QTextEdit* live_log_display;
    QLabel* network_summary_label;
    QLabel* device_summary_label;
    
    // UI Elements - Right Panel (Chat)
    QTextEdit* chat_display;
    QTextEdit* user_input;
    QPushButton* send_button;
    QLabel* processing_label;
    
    // Timer for updates
    QTimer* update_timer;
    // Bridge process to Python backend
    QProcess* bridge_process;
    
    // UI Setup
    void setup_ui();
    void setup_top_bar();
    void setup_left_panel();
    void setup_right_panel();
    void setup_signals();
    
    // UI Update
    void update_status();
    void update_live_log();
    void update_network_summary();
    void update_language();
    void apply_rtl_layout(bool is_rtl);
    
    // Utility
    QString format_timestamp(long timestamp);
    
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
private slots:
    void on_start_collection();
    void on_stop_collection();
    void on_refresh_data();
    void on_send_message();
    void on_language_changed(int index);
    void on_settings_clicked();
    void on_timer_update();
    void on_bridge_stdout();
    void on_bridge_stderr();
    void handle_bridge_message(const QString& line);
    void send_bridge_command(const Json::Value& cmd);
};

#endif // MAIN_WINDOW_H
