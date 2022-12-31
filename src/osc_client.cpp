#include "osc_client.h"

Osc::Client::Client(QHostAddress address, quint16 port) {
  this->_address = address;
  this->_port = port;
  this->_heartbeat = QTime::currentTime();
}
Osc::Client::~Client() {}

QHostAddress Osc::Client::address() { return _address; }

quint16 Osc::Client::port() { return _port; }

QTime Osc::Client::heartbeat() { return _heartbeat; }

void Osc::Client::setHeartbeat() { this->_heartbeat = QTime::currentTime(); }

void Osc::Client::addSubscription(int card_index, int element_index,
                                  int frequency) {
  // TODO: Implement adding a subscription to an element
}

void Osc::Client::removeSubscription(int card_index, int element_index) {
  // TODO: Implement removing an existing subscription to an element
}

Osc::Client::Subscription::Subscription(int card_index, int element_index,
                                        int frequency) {
  // TODO: Implement constructor of a subscription object
}
