#ifndef QML_BRIDGE_H
#define QML_BRIDGE_H

#include <QObject>
#include <QString>
#include <memory>

class NetworkCollector;
class AIEngine;

class QMLBridge : public QObject {
    Q_OBJECT
public:
    QMLBridge(QObject* parent = nullptr);

    void setNetworkCollector(std::shared_ptr<NetworkCollector> nc);
    void setAIEngine(std::shared_ptr<AIEngine> ai);

public slots:
    Q_INVOKABLE bool startCollection();
    Q_INVOKABLE void stopCollection();
    Q_INVOKABLE QString sendQuery(const QString& question);

signals:
    void newLogEntry(const QString& entry);

private:
    std::shared_ptr<NetworkCollector> network_collector;
    std::shared_ptr<AIEngine> ai_engine;
};

#endif // QML_BRIDGE_H
