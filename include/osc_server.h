#pragma once

#include <QHash>
#include <QList>
#include <QNetworkDatagram>
#include <QObject>
#include <QRegularExpression>
#include <QTimer>
#include <QUdpSocket>

#include <functional>
#include <memory>
#include <qhostaddress.h>

#include "alsa_card_model.h"
#include "osc_client.h"
#include "osc_message.h"

namespace Osc {
class Server;

class Endpoint : public QObject {
  Q_OBJECT

public:
  Endpoint(QRegularExpression pattern);
  QRegularExpression pattern;

signals:
  bool received(std::shared_ptr<Osc::Message> received_message,
                Osc::Server *server);
};

class Server : public QObject {
  Q_OBJECT

private:
  Alsa::CardModel *_cardmodel;

  QUdpSocket *_socket;
  bool _enabled = false;
  quint16 _port = 1;

  QList<std::shared_ptr<Osc::Client>> _clients;
  QTimer _heartbeat_timer;

  qint32 _client_heartbeat_timeout_seconds = 5;

  QList<std::shared_ptr<Osc::Endpoint>> _endpoints;

public:
  Server();
  ~Server();

  quint16 port();

  bool enabled();

public slots:
  void setPort(quint16 value);
  void setEnabled(bool value);

  void receiveDatagram();

  void handleNewCard(std::shared_ptr<Alsa::Card> card);

  void
  sendElementUpdateToAllClients(std::shared_ptr<Alsa::Ctl::Element> element);

  void sendOscMessage(std::shared_ptr<Osc::Message> message);

  void sendOscMessageToAllClients(std::shared_ptr<Osc::Message> message);

  void updateClientList();

  void endpoint_heartbeat(std::shared_ptr<Osc::Message> received_message);

  void
  endpoint_element_subscribe(std::shared_ptr<Osc::Message> received_message);
  void
  endpoint_element_unsubscribe(std::shared_ptr<Osc::Message> received_message);

  void endpoint_cards(std::shared_ptr<Osc::Message> received_message);

  void endpoint_card_name(std::shared_ptr<Osc::Message> received_message);
  void endpoint_card_long_name(std::shared_ptr<Osc::Message> received_message);
  void endpoint_card_mixer_name(std::shared_ptr<Osc::Message> received_message);
  void endpoint_card_driver(std::shared_ptr<Osc::Message> received_message);
  void endpoint_card_components(std::shared_ptr<Osc::Message> received_message);
  void endpoint_card_id(std::shared_ptr<Osc::Message> received_message);
  void endpoint_card_sync(std::shared_ptr<Osc::Message> received_message);

  void endpoint_element_count(std::shared_ptr<Osc::Message> received_message);
  void endpoint_element_by_name(std::shared_ptr<Osc::Message> received_message);
  void endpoint_element_name(std::shared_ptr<Osc::Message> received_message);
  void endpoint_element_type(std::shared_ptr<Osc::Message> received_message);
  void endpoint_element_value(std::shared_ptr<Osc::Message> received_message);
  void endpoint_element_minimum(std::shared_ptr<Osc::Message> received_message);
  void endpoint_element_maximum(std::shared_ptr<Osc::Message> received_message);
  void
  endpoint_element_readable(std::shared_ptr<Osc::Message> received_message);
  void
  endpoint_element_writable(std::shared_ptr<Osc::Message> received_message);
  void
  endpoint_element_value_dB(std::shared_ptr<Osc::Message> received_message);
  void
  endpoint_element_minimum_dB(std::shared_ptr<Osc::Message> received_message);
  void
  endpoint_element_maximum_dB(std::shared_ptr<Osc::Message> received_message);
  void
  endpoint_element_enum_list(std::shared_ptr<Osc::Message> received_message);
  void
  endpoint_element_enum_name(std::shared_ptr<Osc::Message> received_message);

signals:
  void portChanged(quint16 value);
  bool enabledChanged(bool value);
};

} // namespace Osc
