#pragma once

#include <QHash>
#include <QList>
#include <QNetworkDatagram>
#include <QObject>
#include <QRegularExpression>
#include <QTimer>
#include <QUdpSocket>

#include "alsa_card_model.h"
#include "osc_action.h"
#include "osc_client.h"
#include "osc_message.h"

namespace Osc {
class Action;

class Server : public QObject {
    Q_OBJECT

private:
    Alsa::CardModel* _cardmodel;

    QHash<int, void*> _mixers;

    QUdpSocket* _socket;
    bool _enabled = false;
    quint16 _port = 1;

    QList<Osc::Action> actions;

    void* get_mixer(int32_t index);

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

    void sendOscMessage(Osc::Message* message);

    void updateClientList();

    void endpoint_heartbeat(QHostAddress remote, quint16 remote_port);

    void endpoint_cards(QHostAddress remote, quint16 remote_port);

    void endpoint_card_all(QHostAddress remote, quint16 remote_port, int card_index);
    void endpoint_card_name(QHostAddress remote, quint16 remote_port, int card_index);
    void endpoint_card_long_name(QHostAddress remote, quint16 remote_port, int card_index);
    void endpoint_card_mixer_name(QHostAddress remote, quint16 remote_port, int card_index);
    void endpoint_card_driver(QHostAddress remote, quint16 remote_port, int card_index);
    void endpoint_card_components(QHostAddress remote, quint16 remote_port, int card_index);
    void endpoint_card_id(QHostAddress remote, quint16 remote_port, int card_index);
    void endpoint_element_count(QHostAddress remote, quint16 remote_port, int card_index);
    void endpoint_element_name(QHostAddress remote, quint16 remote_port, int card_index, int element_index);
    void endpoint_element_volume_playback_available(QHostAddress remote, quint16 remote_port, int card_index, int element_index);
    void endpoint_element_volume_capture_available(QHostAddress remote, quint16 remote_port, int card_index, int element_index);
    void endpoint_element_dB_playback(QHostAddress remote, quint16 remote_port, int card_index, int element_index);
    void endpoint_element_dB_capture(QHostAddress remote, quint16 remote_port, int card_index, int element_index);
    void endpoint_element_dB_playback_min(QHostAddress remote, quint16 remote_port, int card_index, int element_index);
    void endpoint_element_dB_capture_min(QHostAddress remote, quint16 remote_port, int card_index, int element_index);
    void endpoint_element_dB_playback_max(QHostAddress remote, quint16 remote_port, int card_index, int element_index);
    void endpoint_element_dB_capture_max(QHostAddress remote, quint16 remote_port, int card_index, int element_index);

    /*
     * TODO Add slots to reply to all clients
     * TODO Add slots to set volume and other values
     */

signals:
    void portChanged(quint16 value);
    bool enabledChanged(bool value);
};

} // namespace Osc
