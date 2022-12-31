#pragma once

#include <QHostAddress>
#include <QTime>
#include <QTimer>

namespace Osc {
class Client {
public:
  Client(QHostAddress address, quint16 port);
  ~Client();

  QHostAddress address();
  quint16 port();
  QTime heartbeat();
  void setHeartbeat();

  void addSubscription(int card_index, int element_index, int frequency);
  void removeSubscription(int card_index, int element_index);

private:
  class Subscription {
  public:
    Subscription(int card_index, int element_index, int frequency);
    QTimer timer;
    int element_index;
    int card_index;
    int frequency;
  };

  QHostAddress _address;
  quint16 _port;
  QTime _heartbeat;
  QList<Subscription> _subscriptions;
};
} // namespace Osc