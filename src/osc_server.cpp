#include "osc_server.h"

#include <QDebug>
#include <memory>

Osc::Server::Server()
{
    qDebug() << tr("Starting server");

    _heartbeat_timer.setInterval(1000);

    _heartbeat_timer.connect(&_heartbeat_timer, &QTimer::timeout, this, &Osc::Server::updateClientList);

    _heartbeat_timer.start();

    _socket = new QUdpSocket(this);

    connect(_socket, &QUdpSocket::readyRead, this, &Server::receiveDatagram);

    _cardmodel = new Alsa::CardModel(this);
}

Osc::Server::~Server()
{
    qInfo() << "Stopping server";

    _socket->close();

    _socket->deleteLater();
}

bool Osc::Server::enabled()
{
    return _enabled;
}

void Osc::Server::setEnabled(bool value)
{
    if (value != _enabled) {
        _enabled = value;

        if (_enabled) {
            _socket->bind(QHostAddress::LocalHost, _port);
            qInfo() << tr("Binding server to %1:%2").arg(_socket->localAddress().toString()).arg(_port);
        } else {
            _socket->close();
        }
        emit enabledChanged(value);
    }
}

quint16
Osc::Server::port()
{
    return _port;
}

void Osc::Server::setPort(quint16 value)
{
    if (value != _port) {
        _port = value;

        qDebug() << tr("Port set to %1").arg(_port);

        emit portChanged(_port);
    }
}

void Osc::Server::receiveDatagram()
{
    while (_socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = _socket->receiveDatagram();

        std::unique_ptr<Osc::Message> received_message(
            new Osc::Message(&datagram));

        qDebug() << received_message->toString();

        if ((received_message->address == "/heartbeat") && (received_message->format() == "")) {
            endpoint_heartbeat(received_message->sourceAddress, received_message->sourcePort);
        }

        if ((received_message->address == "/cards") && (received_message->format() == "")) {
            endpoint_cards(received_message->sourceAddress, received_message->sourcePort);
        }

        if ((received_message->address == "/card/all") && (received_message->format() == "i")) {
            endpoint_card_all(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt());
        }
        if ((received_message->address == "/card/id") && (received_message->format() == "i")) {
            endpoint_card_id(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt());
        }
        if ((received_message->address == "/card/name") && (received_message->format() == "i")) {
            endpoint_card_name(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt());
        }
        if ((received_message->address == "/card/longname") && (received_message->format() == "i")) {
            endpoint_card_long_name(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt());
        }
        if ((received_message->address == "/card/mixername") && (received_message->format() == "i")) {
            endpoint_card_mixer_name(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt());
        }
        if ((received_message->address == "/card/driver") && (received_message->format() == "i")) {
            endpoint_card_driver(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt());
        }
        if ((received_message->address == "/card/components") && (received_message->format() == "i")) {
            endpoint_card_components(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt());
        }
        if ((received_message->address ==  "/elements") && (received_message->format() == "i")) {
            endpoint_element_count(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt());
        }
        if ((received_message->address == "/element/name") && (received_message->format() == "ii")) {
            endpoint_element_name(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt(), received_message->values[1].toInt());
        }
        if ((received_message->address == "/element/volume/playback/available") && (received_message->format() == "ii")) {
            endpoint_element_volume_playback_available(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt(), received_message->values[1].toInt());
        }
        if ((received_message->address == "/element/volume/capture/available") && (received_message->format() == "ii")) {
            endpoint_element_volume_capture_available(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt(), received_message->values[1].toInt());
        }
        if ((received_message->address == "/element/db/capture") && (received_message->format() == "ii")) {
            endpoint_element_dB_capture(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt(), received_message->values[1].toInt());
        }
        if ((received_message->address == "/element/db/playback") && (received_message->format() == "ii")) {
            endpoint_element_dB_playback(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt(), received_message->values[1].toInt());
        }
        if ((received_message->address == "/element/db/capture/minimum") && (received_message->format() == "ii")) {
            endpoint_element_dB_capture_min(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt(), received_message->values[1].toInt());
        }
        if ((received_message->address == "/element/db/capture/maximum") && (received_message->format() == "ii")) {
            endpoint_element_dB_capture_max(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt(), received_message->values[1].toInt());
        }
        if ((received_message->address == "/element/db/playback/minimum") && (received_message->format() == "ii")) {
            endpoint_element_dB_playback_min(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt(), received_message->values[1].toInt());
        }
        if ((received_message->address == "/element/db/playback/maximum") && (received_message->format() == "ii")) {
            endpoint_element_dB_playback_max(received_message->sourceAddress, received_message->sourcePort, received_message->values[0].toInt(), received_message->values[1].toInt());
        }

    }
}

void Osc::Server::sendOscMessage(Osc::Message* message)
{
    qDebug() << message->toString();

    QNetworkDatagram datagram;

    datagram.setSender(message->sourceAddress, message->sourcePort);
    datagram.setDestination(message->destinationAddress,
        message->destinationPort);

    datagram.setData(message->toByteArray());

    _socket->writeDatagram(datagram);
}

void Osc::Server::updateClientList()
{
    QMutableListIterator<std::shared_ptr<Osc::Client>> clients_iter(this->_clients);
    while (clients_iter.hasNext()) {
        std::shared_ptr<Osc::Client> client = clients_iter.next();

        if (client->heartbeat().addSecs(this->_client_heartbeat_timeout_seconds) < QTime::currentTime()) {
            qInfo() << tr("Client %1:%2 timed out, last heartbeat was at %3").arg(client->address().toString()).arg(client->port()).arg(client->heartbeat().toString());
            clients_iter.remove();
        }
    }
}

void Osc::Server::endpoint_heartbeat(QHostAddress remote, quint16 remote_port)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/heartbeat", QList<QVariant>());

    bool new_client = true;

    QMutableListIterator<std::shared_ptr<Osc::Client>> clients_iter(this->_clients);
    while (clients_iter.hasNext()) {
        std::shared_ptr<Osc::Client> client = clients_iter.next();

        if ((client->address().isEqual(remote)) && (client->port() == remote_port)) {
            client->setHeartbeat();
            new_client = false;
            break;
        }
    };

    if (new_client) {
        qInfo() << tr("New client connected at %1:%2").arg(remote.toString()).arg(remote_port);
        this->_clients.append(std::shared_ptr<Osc::Client> (new Osc::Client(remote, remote_port)));
    }

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_cards(QHostAddress remote, quint16 remote_port)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/cards", QList<QVariant>());

    foreach (int card, _cardmodel->cards()) {
        response.values.append(card);
    }

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_card_all(QHostAddress remote, quint16 remote_port, int card_index)
{
    endpoint_card_id(remote, remote_port, card_index);
    endpoint_card_name(remote, remote_port, card_index);
    endpoint_card_long_name(remote, remote_port, card_index);
    endpoint_card_mixer_name(remote, remote_port, card_index);
    endpoint_card_driver(remote, remote_port, card_index);
    endpoint_card_components(remote, remote_port, card_index);
}

void Osc::Server::endpoint_card_name(QHostAddress remote, quint16 remote_port, int card_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/card/name", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    response.values.append(card_index);
    response.values.append(card->name());

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_card_long_name(QHostAddress remote, quint16 remote_port, int card_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/card/longname", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    response.values.append(card_index);
    response.values.append(card->longName());

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_card_mixer_name(QHostAddress remote, quint16 remote_port, int card_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/card/mixername", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    response.values.append(card_index);
    response.values.append(card->mixerName());

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_card_driver(QHostAddress remote, quint16 remote_port, int card_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/card/driver", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    response.values.append(card_index);
    response.values.append(card->driver());

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_card_components(QHostAddress remote, quint16 remote_port, int card_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/card/components", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    response.values.append(card_index);
    response.values.append(card->components());

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_card_id(QHostAddress remote, quint16 remote_port, int card_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/card/id", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    response.values.append(card_index);
    response.values.append(card->id());

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_element_count(QHostAddress remote, quint16 remote_port, int card_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/elements", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    response.values.append(card_index);

    response.values.append(card->mixer()->count());

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_element_name(QHostAddress remote, quint16 remote_port, int card_index, int element_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/element/name", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    response.values.append(card_index);
    response.values.append(element_index);
    response.values.append(card->mixer()->elementName(element_index));

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_element_volume_playback_available(QHostAddress remote, quint16 remote_port, int card_index, int element_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/element/volume/playback/available", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    if (!card->mixer()->validateIndex(element_index)) {
        return;
    }

    response.values.append(card_index);
    response.values.append(element_index);
    response.values.append(card->mixer()->elementHasPlaybackVolume(element_index));

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_element_volume_capture_available(QHostAddress remote, quint16 remote_port, int card_index, int element_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/element/volume/capture/available", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    if (!card->mixer()->validateIndex(element_index)) {
        return;
    }

    response.values.append(card_index);
    response.values.append(element_index);
    response.values.append(card->mixer()->elementHasCaptureVolume(element_index));

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_element_dB_playback(QHostAddress remote, quint16 remote_port, int card_index, int element_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/element/db/playback", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    if (!card->mixer()->validateIndex(element_index)) {
        return;
    }

    response.values.append(card_index);
    response.values.append(element_index);
    response.values.append(static_cast<int>(card->mixer()->elementPlaybackVolume(element_index)));

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_element_dB_capture(QHostAddress remote, quint16 remote_port, int card_index, int element_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/element/db/capture", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    if (!card->mixer()->validateIndex(element_index)) {
        return;
    }

    response.values.append(card_index);
    response.values.append(element_index);
    response.values.append(static_cast<int>(card->mixer()->elementCaptureVolume(element_index)));

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_element_dB_playback_min(QHostAddress remote, quint16 remote_port, int card_index, int element_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/element/db/playback/minimum", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    if (!card->mixer()->validateIndex(element_index)) {
        return;
    }

    response.values.append(card_index);
    response.values.append(element_index);
    response.values.append(static_cast<int>(card->mixer()->elementPlaybackVolumeMinimum(element_index)));

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_element_dB_capture_min(QHostAddress remote, quint16 remote_port, int card_index, int element_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/element/db/capture/minimum", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    if (!card->mixer()->validateIndex(element_index)) {
        return;
    }

    response.values.append(card_index);
    response.values.append(element_index);
    response.values.append(static_cast<int>(card->mixer()->elementCaptureVolumeMinimum(element_index)));

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_element_dB_playback_max(QHostAddress remote, quint16 remote_port, int card_index, int element_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/element/db/playback/maximum", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    if (!card->mixer()->validateIndex(element_index)) {
        return;
    }

    response.values.append(card_index);
    response.values.append(element_index);
    response.values.append(static_cast<int>(card->mixer()->elementPlaybackVolumeMaximum(element_index)));

    this->sendOscMessage(&response);
}

void Osc::Server::endpoint_element_dB_capture_max(QHostAddress remote, quint16 remote_port, int card_index, int element_index)
{
    Osc::Message response(remote, remote_port, this->_socket->localAddress(), this->_socket->localPort(), "/element/db/capture/maximum", QList<QVariant>());

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
        qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
        return;
    }

    if (!card->mixer()->validateIndex(element_index)) {
        return;
    }

    response.values.append(card_index);
    response.values.append(element_index);
    response.values.append(static_cast<int>(card->mixer()->elementCaptureVolumeMaximum(element_index)));

    this->sendOscMessage(&response);
}
