#ifndef OSC_SERVER_H
#define OSC_SERVER_H

#include <QHash>
#include <QList>
#include <QNetworkDatagram>
#include <QObject>
#include <QRegularExpression>
#include <QTimer>
#include <QUdpSocket>

#include "alsa_card_model.h"
#include "osc_client.h"
#include "osc_message.h"

namespace Osc {
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

  void sendOscMessage(Osc::Message *message);
  void sendOscMessageToAllClients(Osc::Message *message);
  void sendOscMessageToAllClients(std::shared_ptr<Osc::Message> message);

  void updateClientList();

  void endpoint_heartbeat(QHostAddress remote, quint16 remote_port);

  void endpoint_cards(QHostAddress remote, quint16 remote_port);

  void endpoint_card_all(QHostAddress remote, quint16 remote_port,
                         int card_index);
  void endpoint_card_name(QHostAddress remote, quint16 remote_port,
                          int card_index);
  void endpoint_card_long_name(QHostAddress remote, quint16 remote_port,
                               int card_index);
  void endpoint_card_mixer_name(QHostAddress remote, quint16 remote_port,
                                int card_index);
  void endpoint_card_driver(QHostAddress remote, quint16 remote_port,
                            int card_index);
  void endpoint_card_components(QHostAddress remote, quint16 remote_port,
                                int card_index);
  void endpoint_card_id(QHostAddress remote, quint16 remote_port,
                        int card_index);

  void endpoint_element_count(QHostAddress remote, quint16 remote_port,
                              int card_index);
  void endpoint_element_by_name(QHostAddress remote, quint16 remote_port,
                                int card_index, QString element_name);
  void endpoint_element_all(QHostAddress remote, quint16 remote_port,
                            int card_index, int element_index);

  void endpoint_element_name(QHostAddress remote, quint16 remote_port,
                             int card_index, int element_index);
  void endpoint_element_type(QHostAddress remote, quint16 remote_port,
                             int card_index, int element_index);
  void endpoint_element_value(QHostAddress remote, quint16 remote_port,
                              int card_index, int element_index);
  void endpoint_element_value(QHostAddress remote, quint16 remote_port,
                              int card_index, int element_index, int value);
  void endpoint_element_minimum(QHostAddress remote, quint16 remote_port,
                                int card_index, int element_index);
  void endpoint_element_maximum(QHostAddress remote, quint16 remote_port,
                                int card_index, int element_index);
  void endpoint_element_readable(QHostAddress remote, quint16 remote_port,
                                 int card_index, int element_index);
  void endpoint_element_writable(QHostAddress remote, quint16 remote_port,
                                 int card_index, int element_index);
  void endpoint_element_value_dB(QHostAddress remote, quint16 remote_port,
                                 int card_index, int element_index);
  void endpoint_element_minimum_dB(QHostAddress remote, quint16 remote_port,
                                   int card_index, int element_index);
  void endpoint_element_maximum_dB(QHostAddress remote, quint16 remote_port,
                                   int card_index, int element_index);

signals:
  void portChanged(quint16 value);
  bool enabledChanged(bool value);
};

} // namespace Osc

#endif
