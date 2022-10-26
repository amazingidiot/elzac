#pragma once

#include <QHostAddress>
#include <QTime>

namespace Osc {
class Client {
public:
    Client(QHostAddress address, quint16 port);
    ~Client();

    QHostAddress address();
    quint16 port();
    QTime heartbeat();
    void setHeartbeat();

private:
    QHostAddress _address;
    quint16 _port;
    QTime _heartbeat;
};
} // namespace Osc
