#include "qml_bridge.h"
#include <json/json.h>
#include <memory>
#include <QString>
#include <iostream>
#include <ctime>

#include "network_collector.h"
#include "ai_engine.h"

QMLBridge::QMLBridge(QObject* parent) : QObject(parent) {}

void QMLBridge::setNetworkCollector(std::shared_ptr<NetworkCollector> nc) {
    network_collector = nc;
}

void QMLBridge::setAIEngine(std::shared_ptr<AIEngine> ai) {
    ai_engine = ai;
}

bool QMLBridge::startCollection() {
    if (!network_collector) return false;
    bool ok = network_collector->start_collection();
    if (ok) emit newLogEntry(QString::fromStdString("[" + network_collector->format_timestamp(time(nullptr)) + "] Data collection started (QML)"));
    return ok;
}

void QMLBridge::stopCollection() {
    if (!network_collector) return;
    network_collector->stop_collection();
    emit newLogEntry(QString::fromStdString("[" + network_collector->format_timestamp(time(nullptr)) + "] Data collection stopped (QML)"));
}

QString QMLBridge::sendQuery(const QString& question) {
    if (!ai_engine || !network_collector) return "AI or network unavailable";
    Json::Value network_json = network_collector->to_json();
    std::string resp = ai_engine->query(question.toStdString(), network_json);
    emit newLogEntry(QString::fromStdString("[" + network_collector->format_timestamp(time(nullptr)) + "] Query sent: " ) + question);
    return QString::fromStdString(resp);
}

