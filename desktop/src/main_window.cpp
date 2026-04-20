#include "main_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QApplication>
#include <QStyle>
#include <QDateTime>
#include <iostream>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QIcon>
#include <QFile>

MainWindow::MainWindow(QWidget* parent) 
    : QMainWindow(parent) {
    
    // Ensure we're in the correct working directory (where bridge, models, languages are)
    // Try to find and switch to the directory containing desktop_py
    QDir currentDir = QDir::current();
    if (!currentDir.exists("desktop_py")) {
        // Try parent directory
        currentDir.cdUp();
        if (currentDir.exists("desktop_py")) {
            QDir::setCurrent(currentDir.path());
        }
    }
    
    // Load and apply icon: try multiple fallback locations
    QStringList iconCandidates = {"icon.svg", "resources/icon.svg", "../resources/icon.svg", "build/icon.svg", "build/icon.ico", "resources/icon.ico"};
    for (const QString &p : iconCandidates) {
        if (QFile::exists(p)) {
            this->setWindowIcon(QIcon(p));
            break;
        }
    }
    
    // Initialize core engines
    network_collector = std::make_unique<NetworkCollector>();
    data_normalizer = std::make_unique<DataNormalizer>();
    ai_engine = std::make_unique<AIEngine>("models/mistrallite.Q2_K.gguf");
    language_manager = std::make_unique<LanguageManager>("core/languages/");
    // Start Python bridge process for backend (desktop_py/bridge.py)
    bridge_process = new QProcess(this);
    QString python = "python3";
    QStringList args;
    // Try several common locations for the bridge script depending on working dir
    if (QFile::exists("desktop_py/bridge.py")) {
        args << "desktop_py/bridge.py";
    } else if (QFile::exists("build/desktop_py/bridge.py")) {
        args << "build/desktop_py/bridge.py";
    } else if (QFile::exists("../desktop_py/bridge.py")) {
        args << "../desktop_py/bridge.py";
    } else {
        args << "desktop_py/bridge.py"; // fallback, child will error if not found
    }
    bridge_process->setProcessChannelMode(QProcess::MergedChannels);
    bridge_process->start(python, args);
    connect(bridge_process, &QProcess::readyReadStandardOutput, this, &MainWindow::on_bridge_stdout);
    connect(bridge_process, &QProcess::readyReadStandardError, this, &MainWindow::on_bridge_stderr);
    
    // Initialize AI engine
    ai_engine->initialize();
    
    // Setup UI
    setup_ui();
    setup_signals();
    
    // Setup update timer
    update_timer = new QTimer(this);
    connect(update_timer, &QTimer::timeout, this, &MainWindow::on_timer_update);
    update_timer->start(1000); // Update every second
    
    // Set window properties
    setWindowTitle("LINA - Learning Interconnected Network Analyzer");
    setGeometry(100, 100, 1400, 800);
}

MainWindow::~MainWindow() {
    if (network_collector && network_collector->is_active()) {
        network_collector->stop_collection();
    }
}

void MainWindow::setup_ui() {
    // Create central widget
    QWidget* central_widget = new QWidget(this);
    QVBoxLayout* main_layout = new QVBoxLayout(central_widget);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);
    
    // Setup top bar
    setup_top_bar();
    
    // Main splitter
    QSplitter* main_splitter = new QSplitter(Qt::Horizontal);
    main_splitter->setStyleSheet("QSplitter { background-color: #1A1A1A; }");
    
    // Left panel
    QWidget* left_panel = new QWidget();
    left_panel->setStyleSheet("QWidget { background-color: #1A1A1A; }");
    left_panel->setMaximumWidth(300);
    left_panel->setMinimumWidth(280);
    QVBoxLayout* left_layout = new QVBoxLayout(left_panel);
    left_layout->setContentsMargins(16, 16, 16, 16);
    left_layout->setSpacing(12);
    setup_left_panel();
    left_layout->addWidget(status_indicator);
    left_layout->addSpacing(8);
    left_layout->addWidget(new QLabel("CONTROL BUTTONS"), 0, Qt::AlignLeft);
    left_layout->addWidget(start_button);
    left_layout->addWidget(stop_button);
    left_layout->addWidget(refresh_button);
    left_layout->addSpacing(8);
    left_layout->addWidget(new QLabel("LIVE METADATA LOG"), 0, Qt::AlignLeft);
    left_layout->addWidget(live_log_display);
    left_layout->addSpacing(8);
    left_layout->addWidget(new QLabel("NETWORK SUMMARY"), 0, Qt::AlignLeft);
    left_layout->addWidget(network_summary_label);
    left_layout->addStretch();
    
    // Right panel
    QWidget* right_panel = new QWidget();
    right_panel->setStyleSheet("QWidget { background-color: #1A1A1A; }");
    QVBoxLayout* right_layout = new QVBoxLayout(right_panel);
    right_layout->setContentsMargins(16, 16, 16, 16);
    right_layout->setSpacing(12);
    setup_right_panel();
    // Processing indicator (hidden by default) - placed in UI where right_layout exists
    processing_label = new QLabel("LINA: Memproses...");
    processing_label->setStyleSheet("QLabel { color: #F1C40F; font-size: 13px; font-weight: bold; }");
    processing_label->setVisible(false);
    right_layout->addWidget(processing_label, 0, Qt::AlignLeft);
    right_layout->addWidget(new QLabel("Chat with LINA"), 0, Qt::AlignLeft);
    right_layout->addWidget(chat_display);
    right_layout->addSpacing(8);
    right_layout->addWidget(new QLabel("Your Question:"), 0, Qt::AlignLeft);
    right_layout->addWidget(user_input);
    right_layout->addWidget(send_button);
    
    // Add panels to splitter
    main_splitter->addWidget(left_panel);
    main_splitter->addWidget(right_panel);
    main_splitter->setStretchFactor(0, 0);
    main_splitter->setStretchFactor(1, 1);
    main_splitter->setCollapsible(0, false);
    
    main_layout->addWidget(main_splitter);
    setCentralWidget(central_widget);
    
    // Load initial language
    language_manager->set_language("en_US");
    update_language();
}

void MainWindow::setup_top_bar() {
    // Top bar widget
    QWidget* top_bar = new QWidget();
    QHBoxLayout* top_layout = new QHBoxLayout(top_bar);
    
    // App title
    app_title_label = new QLabel("LINA");
    app_title_label->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #ECF0F1; }");
    top_layout->addWidget(app_title_label);
    
    // Spacer
    top_layout->addStretch();
    
    // Settings button
    settings_button = new QPushButton("⚙️ Settings");
    settings_button->setMaximumWidth(100);
    top_layout->addWidget(settings_button);
    
    // Language selector
    language_selector = new QComboBox();
    language_selector->addItem("🇮🇩 Indonesian", "id_ID");
    language_selector->addItem("🇬🇧 English", "en_US");
    language_selector->addItem("🇸🇦 العربية", "ar_SA");
    language_selector->setMaximumWidth(150);
    top_layout->addWidget(language_selector);
    
    // Styling
    top_bar->setStyleSheet("QWidget { background-color: #2C3E50; }");
    
    // Add to main window
    QWidget* top_widget = new QWidget();
    QVBoxLayout* top_main = new QVBoxLayout(top_widget);
    top_main->setContentsMargins(16, 8, 16, 8);
    top_main->addWidget(top_bar);
    
    QMainWindow::setMenuWidget(top_widget);
}

void MainWindow::setup_left_panel() {
    // Status indicator
    status_indicator = new QLabel("🟢 Idle");
    status_indicator->setStyleSheet("QLabel { color: #27AE60; font-size: 16px; font-weight: bold; }");
    
    // Control buttons
    start_button = new QPushButton("▶️  START COLLECTION");
    stop_button = new QPushButton("⏹️  STOP COLLECTION");
    refresh_button = new QPushButton("🔄  REFRESH NOW");
    
    start_button->setMinimumHeight(44);
    stop_button->setMinimumHeight(44);
    refresh_button->setMinimumHeight(44);
    
    start_button->setStyleSheet("QPushButton { background-color: #27AE60; color: #ECF0F1; border: none; border-radius: 4px; font-weight: bold; } QPushButton:hover { background-color: #229954; } QPushButton:pressed { background-color: #1E8449; }");
    stop_button->setStyleSheet("QPushButton { background-color: #E74C3C; color: #ECF0F1; border: none; border-radius: 4px; font-weight: bold; } QPushButton:hover { background-color: #C0392B; } QPushButton:pressed { background-color: #A93226; }");
    refresh_button->setStyleSheet("QPushButton { background-color: #3498DB; color: #ECF0F1; border: none; border-radius: 4px; font-weight: bold; } QPushButton:hover { background-color: #2980B9; } QPushButton:pressed { background-color: #2471A3; }");
    
    stop_button->setEnabled(false);
    
    // Live log
    live_log_display = new QTextEdit();
    live_log_display->setReadOnly(true);
    live_log_display->setPlaceholderText("Live metadata log will appear here...");
    live_log_display->setMaximumHeight(200);
    live_log_display->setMinimumHeight(100);
    live_log_display->setStyleSheet("QTextEdit { background-color: #0D0D0D; color: #ECF0F1; border: 1px solid #222222; border-radius: 4px; padding: 8px; font-family: 'Courier New'; font-size: 11px; }");
    
    // Network summary
    network_summary_label = new QLabel("Networks: 0\nDevices: 0\nAvg Signal: N/A");
    network_summary_label->setStyleSheet("QLabel { color: #ECF0F1; font-size: 13px; line-height: 1.6; }");
    
    // Device summary
    device_summary_label = new QLabel("Device Types:\n(Updating...)");
    device_summary_label->setStyleSheet("QLabel { color: #ECF0F1; font-size: 13px; line-height: 1.6; }");
}

void MainWindow::setup_right_panel() {
    // Chat display
    chat_display = new QTextEdit();
    chat_display->setReadOnly(true);
    chat_display->setPlaceholderText("Chat with LINA will appear here...");
    chat_display->setStyleSheet("QTextEdit { background-color: #0D0D0D; color: #ECF0F1; border: 1px solid #222222; border-radius: 4px; padding: 12px; font-size: 13px; }");
    
    // User input
    user_input = new QTextEdit();
    user_input->setPlaceholderText("Type your question here...");
    user_input->setMaximumHeight(100);
    user_input->setMinimumHeight(60);
    user_input->setStyleSheet("QTextEdit { background-color: #0D0D0D; color: #ECF0F1; border: 1px solid #222222; border-radius: 4px; padding: 10px; font-size: 13px; }");
    
    // Send button
    send_button = new QPushButton("📤 SEND MESSAGE");
    send_button->setMinimumHeight(44);
    send_button->setMaximumHeight(44);
    send_button->setStyleSheet("QPushButton { background-color: #3498DB; color: #ECF0F1; border: none; border-radius: 4px; font-weight: bold; font-size: 13px; } QPushButton:hover { background-color: #2980B9; } QPushButton:pressed { background-color: #2471A3; }");
}

void MainWindow::setup_signals() {
    connect(start_button, &QPushButton::clicked, this, &MainWindow::on_start_collection);
    connect(stop_button, &QPushButton::clicked, this, &MainWindow::on_stop_collection);
    connect(refresh_button, &QPushButton::clicked, this, &MainWindow::on_refresh_data);
    connect(send_button, &QPushButton::clicked, this, &MainWindow::on_send_message);
    connect(language_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::on_language_changed);
    connect(settings_button, &QPushButton::clicked, this, &MainWindow::on_settings_clicked);
}


// Read lines from bridge stdout and handle JSON messages
void MainWindow::on_bridge_stdout() {
    if (!bridge_process) return;
    QByteArray out = bridge_process->readAllStandardOutput();
    QList<QByteArray> lines = out.split('\n');
    for (const QByteArray &b : lines) {
        if (b.trimmed().isEmpty()) continue;
        QString s = QString::fromUtf8(b).trimmed();
        handle_bridge_message(s);
    }
}

void MainWindow::on_bridge_stderr() {
    if (!bridge_process) return;
    QByteArray err = bridge_process->readAllStandardError();
    if (!err.isEmpty()) {
        live_log_display->append("[Bridge STDERR] " + QString::fromUtf8(err));
    }
}

void MainWindow::handle_bridge_message(const QString& line) {
    QJsonParseError perr;
    QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8(), &perr);
    if (perr.error != QJsonParseError::NoError) {
        // Silently ignore JSON parse errors (likely non-JSON output from bridge or polling events)
        // To enable debug: uncomment line below
        // live_log_display->append("[Bridge] invalid json: " + perr.errorString());
        return;
    }
    if (!doc.isObject()) return;
    QJsonObject obj = doc.object();
    QString event = obj.value("event").toString();
    if (event == "collected") {
        // Update summary
        QJsonObject data = obj.value("data").toObject();
        int networks = 0, devices = 0;
        if (data.contains("networks") && data.value("networks").isArray()) networks = data.value("networks").toArray().size();
        if (data.contains("devices") && data.value("devices").isArray()) devices = data.value("devices").toArray().size();
        QString summary = "Networks: " + QString::number(networks) + "\n" + "Devices: " + QString::number(devices);
        network_summary_label->setText(summary);
        live_log_display->append("[" + format_timestamp(QDateTime::currentSecsSinceEpoch()) + "] Data collected via bridge");
    } else if (event == "ai_opening") {
        QString text = obj.value("text").toString();
        chat_display->append("← LINA (auto):\n" + text + "\n");
        if (processing_label) processing_label->setVisible(false);
    } else if (event == "ai_response") {
        QString text = obj.value("text").toString();
        QString question = obj.value("question").toString();
        chat_display->append("← LINA (response to: " + question + "):\n" + text + "\n");
        if (processing_label) processing_label->setVisible(false);
    } else if (event == "error") {
        QString msg = obj.value("message").toString();
        live_log_display->append("[Bridge Error] " + msg);
        if (processing_label) processing_label->setVisible(false);
    }
}

void MainWindow::send_bridge_command(const Json::Value& cmd) {
    if (!bridge_process) return;
    // Use compact JSON to avoid potential newline/formatting issues
    Json::StreamWriterBuilder w;
    w["indentation"] = ""; // compact
    std::string out = Json::writeString(w, cmd);
    QByteArray ba = QByteArray::fromStdString(out + "\n");
    qint64 written = bridge_process->write(ba);
    // Wait a short moment for bytes to be written to child stdin
    bridge_process->waitForBytesWritten(200);
    if (written <= 0) {
        // do not spam UI; log to live_log_display minimally
        live_log_display->append("[Bridge SEND] write failed or 0 bytes written");
    }
}

void MainWindow::on_start_collection() {
    // Request the Python bridge to perform a passive, read-only collection
    Json::Value cmd;
    cmd["cmd"] = "collect";
    send_bridge_command(cmd);
    status_indicator->setText("🟢 Active (Collecting data)");
    status_indicator->setStyleSheet("QLabel { color: #27AE60; font-size: 14px; font-weight: bold; }");
    start_button->setEnabled(false);
    stop_button->setEnabled(true);
    live_log_display->append("[" + format_timestamp(QDateTime::currentSecsSinceEpoch()) + "] Requested collection via bridge");
}

void MainWindow::on_stop_collection() {
    // For passive collector the stop is a UI toggle; inform user
    status_indicator->setText("🟡 Idle");
    status_indicator->setStyleSheet("QLabel { color: #F39C12; font-size: 14px; }");
    start_button->setEnabled(true);
    stop_button->setEnabled(false);
    live_log_display->append("[" + format_timestamp(QDateTime::currentSecsSinceEpoch()) + "] Collection stopped (bridge remains running)");
}

void MainWindow::on_refresh_data() {
    // Request a fresh passive collection via the Python bridge
    Json::Value cmd;
    cmd["cmd"] = "collect";
    send_bridge_command(cmd);
    live_log_display->append("[" + format_timestamp(QDateTime::currentSecsSinceEpoch()) + "] Requested refresh via bridge");
}

void MainWindow::on_send_message() {
    QString question = user_input->toPlainText().trimmed();
    
    if (question.isEmpty()) return;
    
    // Add user message to chat
    chat_display->append("→ You (" + format_timestamp(QDateTime::currentSecsSinceEpoch()) + "):\n" + question + "\n");
    user_input->clear();
    // Forward question to Python bridge which uses the latest collected data
    Json::Value cmd;
    cmd["cmd"] = "query";
    cmd["question"] = question.toStdString();
    // Show processing indicator and send command
    if (processing_label) processing_label->setVisible(true);
    send_bridge_command(cmd);
}

void MainWindow::on_language_changed(int index) {
    QString locale = language_selector->currentData().toString();
    language_manager->set_language(locale.toStdString());
    update_language();
    apply_rtl_layout(language_manager->is_rtl());
}

void MainWindow::on_settings_clicked() {
    // TODO: Open settings dialog
}

void MainWindow::on_timer_update() {
    update_status();
    // Poll for new events from network collector and append to live log
    if (network_collector) {
        auto events = network_collector->pop_events();
        for (const auto &e : events) {
            live_log_display->append(QString::fromStdString(e));
        }
    }
}

void MainWindow::update_status() {
    if (network_collector->is_active()) {
        // Update status and summaries periodically
        update_network_summary();
    }
}

void MainWindow::update_live_log() {
    auto events = network_collector->get_events();
    // Display last 10 events
}

void MainWindow::update_network_summary() {
    auto data = network_collector->get_latest_data();
    
    QString summary = "Networks: " + QString::number(data.networks.size()) + "\n";
    summary += "Devices: " + QString::number(data.devices.size()) + "\n";
    
    if (!data.networks.empty()) {
        int total_signal = 0;
        for (const auto& net : data.networks) {
            total_signal += net.signal_strength;
        }
        int avg_signal = total_signal / data.networks.size();
        summary += "Avg Signal: " + QString::number(avg_signal) + " dBm";
    }
    
    network_summary_label->setText(summary);
}

void MainWindow::update_language() {
    app_title_label->setText(QString::fromStdString(language_manager->get("app_name")));
    start_button->setText(QString::fromStdString(language_manager->get("btn_start")));
    stop_button->setText(QString::fromStdString(language_manager->get("btn_stop")));
    refresh_button->setText(QString::fromStdString(language_manager->get("btn_refresh")));
    send_button->setText(QString::fromStdString(language_manager->get("btn_send")));
    settings_button->setText(QString::fromStdString(language_manager->get("btn_settings")));
}

void MainWindow::apply_rtl_layout(bool is_rtl) {
    if (is_rtl) {
        qApp->setLayoutDirection(Qt::RightToLeft);
    } else {
        qApp->setLayoutDirection(Qt::LeftToRight);
    }
}

QString MainWindow::format_timestamp(long timestamp) {
    return QDateTime::fromSecsSinceEpoch(timestamp).toString("hh:mm:ss");
}
