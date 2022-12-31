#include "osc_server.h"
#include "alsa_card_model.h"
#include "osc_client.h"
#include "osc_message.h"

#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentFilter>
#include <memory>

Osc::Endpoint::Endpoint(QRegularExpression pattern) { this->pattern = pattern; }

Osc::Server::Server() {
  qDebug() << tr("Starting server");

  _heartbeat_timer.setInterval(1000);

  _heartbeat_timer.connect(&_heartbeat_timer, &QTimer::timeout, this,
                           &Osc::Server::updateClientList);

  _heartbeat_timer.start();

// Setting up endpoints
#pragma region
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/heartbeat$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_heartbeat);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/cards$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_cards);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/card/all$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_id);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_name);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_mixer_name);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_driver);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_components);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_long_name);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/card/id$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_id);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/card/name$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_name);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/card/longname$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_long_name);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/card/mixername$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_mixer_name);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/card/driver$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_driver);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/card/components$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_components);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/card/sync$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_card_sync);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/elements$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_count);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/by-name$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_by_name);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/all$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_name);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_type);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_value);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_minimum);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_maximum);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_readable);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_writable);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_value_dB);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_minimum_dB);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_maximum_dB);
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_enum_list);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/name$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_name);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/type$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_type);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/value$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_value);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/minimum$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_minimum);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/maximum$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_maximum);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/readable$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_readable);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/writable$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_writable);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/db/value$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_value_dB);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/db/minimum$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_minimum_dB);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/db/maximum$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_maximum_dB);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/enum/list$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_enum_list);
    _endpoints.append(endpoint);
  }
  {
    std::shared_ptr<Osc::Endpoint> endpoint(
        new Osc::Endpoint(QRegularExpression("^/element/enum/name$")));
    QObject::connect(endpoint.get(), &Osc::Endpoint::received, this,
                     &Osc::Server::endpoint_element_enum_name);
    _endpoints.append(endpoint);
  }
#pragma endregion

  _socket = new QUdpSocket(this);

  connect(_socket, &QUdpSocket::readyRead, this, &Server::receiveDatagram);

  _cardmodel = new Alsa::CardModel(this);

  connect(_cardmodel, &Alsa::CardModel::cardCreated, this,
          &Osc::Server::handleNewCard);

  _cardmodel->updateCards();
}

Osc::Server::~Server() {
  qInfo() << "Stopping server";

  _socket->close();

  _socket->deleteLater();
}

bool Osc::Server::enabled() { return _enabled; }

void Osc::Server::setEnabled(bool value) {
  if (value != _enabled) {
    _enabled = value;

    if (_enabled) {
      _socket->bind(QHostAddress::LocalHost, _port);
      qInfo() << tr("Binding server to %1:%2")
                     .arg(_socket->localAddress().toString())
                     .arg(_port);
    } else {
      _socket->close();
    }
    emit enabledChanged(value);
  }
}

quint16 Osc::Server::port() { return _port; }

void Osc::Server::setPort(quint16 value) {
  if (value != _port) {
    _port = value;

    qDebug() << tr("Port set to %1").arg(_port);

    emit portChanged(_port);
  }
}

void Osc::Server::receiveDatagram() {
  while (_socket->hasPendingDatagrams()) {
    QNetworkDatagram datagram = _socket->receiveDatagram();

    std::shared_ptr<Osc::Message> received_message(new Osc::Message(&datagram));

    qDebug() << received_message->toString();

    // TODO: Replace _endpoints with concurrently filtered list
    foreach (std::shared_ptr<Osc::Endpoint> endpoint, _endpoints) {
      if (endpoint->pattern.match(received_message->address).hasMatch()) {
        emit endpoint->received(received_message, this);
      }
    }
  }
}

void Osc::Server::handleNewCard(std::shared_ptr<Alsa::Card> card) {
  qDebug() << tr("handling new card");

  connect(card->elementList().get(), &Alsa::Ctl::ElementList::elementChanged,
          this, &Osc::Server::sendElementUpdateToAllClients);
}

void Osc::Server::sendElementUpdateToAllClients(
    std::shared_ptr<Alsa::Ctl::Element> element) {
  qDebug() << tr("Sending update of element %1:%2")
                  .arg(element->card_index())
                  .arg(element->id());

  QList<QVariant> values;

  values.append(element->card_index());
  values.append(element->id());
  values.append(element->value());

  std::shared_ptr<Osc::Message> message(
      new Osc::Message("/element/value", values));

  this->sendOscMessageToAllClients(message);
}

void Osc::Server::sendOscMessage(std::shared_ptr<Osc::Message> message) {
  qDebug() << message->toString();

  QNetworkDatagram datagram;

  datagram.setSender(_socket->localAddress(), _socket->localPort());
  datagram.setDestination(message->destinationAddress,
                          message->destinationPort);

  datagram.setData(message->toByteArray());

  _socket->writeDatagram(datagram);
}

void Osc::Server::sendOscMessageToAllClients(
    std::shared_ptr<Osc::Message> message) {
  std::shared_ptr<Osc::Message> client_message(
      new Osc::Message(message->address, message->values));

  foreach (std::shared_ptr<Osc::Client> client, _clients) {
    client_message->destinationAddress = client->address();
    client_message->destinationPort = client->port();

    this->sendOscMessage(client_message);
  }
}

void Osc::Server::updateClientList() {
  QMutableListIterator<std::shared_ptr<Osc::Client>> clients_iter(
      this->_clients);
  while (clients_iter.hasNext()) {
    std::shared_ptr<Osc::Client> client = clients_iter.next();

    if (client->heartbeat().addSecs(this->_client_heartbeat_timeout_seconds) <
        QTime::currentTime()) {
      qInfo() << tr("Client %1:%2 timed out, last heartbeat was at %3")
                     .arg(client->address().toString())
                     .arg(client->port())
                     .arg(client->heartbeat().toString());
      clients_iter.remove();
    }
  }
}

void Osc::Server::endpoint_heartbeat(
    std::shared_ptr<Osc::Message> received_message) {
  std::shared_ptr<Osc::Message> response = received_message->response();

  for (int i = 0; i < _clients.count(); i++) {
    std::shared_ptr<Osc::Client> client = _clients.at(i);

    sendOscMessage(response);

    if (client->address().isEqual(received_message->sourceAddress) &&
        (client->port() == received_message->sourcePort)) {
      client->setHeartbeat();

      return;
    }
  }

  qInfo() << tr("New client connected at %1:%2")
                 .arg(received_message->sourceAddress.toString())
                 .arg(received_message->sourcePort);
  _clients.append(std::shared_ptr<Osc::Client>(new Osc::Client(
      received_message->sourceAddress, received_message->sourcePort)));
}

void Osc::Server::endpoint_element_subscribe(
    std::shared_ptr<Osc::Message> received_message) {}

void Osc::Server::endpoint_element_unsubscribe(
    std::shared_ptr<Osc::Message> received_message) {}

void Osc::Server::endpoint_cards(
    std::shared_ptr<Osc::Message> received_message) {
  std::shared_ptr<Osc::Message> response = received_message->response("/cards");

  foreach (int card, _cardmodel->cards()) {
    response->values.append(card);
  }

  sendOscMessage(response);
}

void Osc::Server::endpoint_card_name(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "i") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/card/name");

    int card_index = received_message->values[0].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(card->name());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_card_long_name(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "i") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/card/longname");

    int card_index = received_message->values[0].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(card->longName());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_card_mixer_name(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "i") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/card/mixername");

    int card_index = received_message->values[0].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(card->mixerName());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_card_driver(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "i") {

    std::shared_ptr<Osc::Message> response =
        received_message->response("/card/driver");

    int card_index = received_message->values[0].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(card->driver());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_card_components(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "i") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/card/components");

    int card_index = received_message->values[0].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(card->components());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_card_id(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "i") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/card/id");

    int card_index = received_message->values[0].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(card->id());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_card_sync(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "i") {
    int card_index = received_message->values[0].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }

    std::shared_ptr<Osc::Message> element_message(
        new Osc::Message("/element/all", QList<QVariant>()));
    element_message->sourceAddress = received_message->sourceAddress;
    element_message->sourcePort = received_message->sourcePort;

    element_message->values.append(card_index);
    element_message->values.append(0);

    for (int i = 0; i < card->elementList()->count(); i++) {
      element_message->values[1] = i;

      endpoint_element_name(element_message);
      endpoint_element_type(element_message);
      endpoint_element_value(element_message);
      endpoint_element_minimum(element_message);
      endpoint_element_maximum(element_message);
      endpoint_element_readable(element_message);
      endpoint_element_writable(element_message);
      endpoint_element_value_dB(element_message);
      endpoint_element_minimum_dB(element_message);
      endpoint_element_maximum_dB(element_message);
      endpoint_element_enum_list(element_message);
    }
  }
}

void Osc::Server::endpoint_element_count(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "i") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/count");

    int card_index = received_message->values[0].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }

    response->values.append(card_index);

    response->values.append(card->elementList()->count());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_element_name(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "ii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/name");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);
    response->values.append(element->name());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_element_type(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "ii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/type");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);
    response->values.append(element->type());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_element_value(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "ii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/value");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);

    qDebug() << tr("Requested element %1 has %2 channels")
                    .arg(element_index)
                    .arg(element->channel_count());

    for (int i = 0; i < element->channel_count(); i++) {
      response->values.append(element->value(i));
    }
    sendOscMessage(response);
  } else if (received_message->format() == "iii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/value");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();
    int value = received_message->values[2].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);
    qDebug() << tr("Setting element %1 to %2").arg(element_index).arg(value);

    for (int i = 0; i < element->channel_count(); i++) {
      element->setValue(value, i);
    }
  }
}

void Osc::Server::endpoint_element_minimum(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "ii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/minimum");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);
    response->values.append(element->min());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_element_maximum(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "ii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/maximum");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);
    response->values.append(element->max());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_element_readable(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "ii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/readable");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);
    response->values.append(element->isReadable());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_element_writable(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "ii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/writable");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);
    response->values.append(element->isWritable());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_element_value_dB(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "ii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/db/value");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);

    qDebug() << tr("Requested element %1 has %2 channels")
                    .arg(element_index)
                    .arg(element->channel_count());

    for (int i = 0; i < element->channel_count(); i++) {
      response->values.append(element->valuedB(i));
    }
    sendOscMessage(response);
  } else if (received_message->format() == "iii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/db/value");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();
    int value = received_message->values[2].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);
    qDebug() << tr("Setting element %1 to %2").arg(element_index).arg(value);

    for (int i = 0; i < element->channel_count(); i++) {
      element->setValuedB(value, i);
    }
  }
}

void Osc::Server::endpoint_element_minimum_dB(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "ii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/db/minimum");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);
    response->values.append(element->mindB());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_element_maximum_dB(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "ii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/db/maximum");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);
    response->values.append(element->maxdB());

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_element_enum_list(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "ii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/enum/list");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);
    foreach (QString value, element->enum_list()) {
      response->values.append(value);
    }

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_element_enum_name(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "iii") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/enum/name");

    int card_index = received_message->values[0].toInt();
    int element_index = received_message->values[1].toInt();
    int enum_index = received_message->values[2].toInt();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByID(element_index);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with id '%1' not found").arg(element_index);
      return;
    }

    if (element->type() != 3) {
      qWarning()
          << tr("Element %1:%2 is no enum").arg(card_index).arg(element_index);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_index);

    response->values.append(element->enum_name(enum_index));

    sendOscMessage(response);
  }
}

void Osc::Server::endpoint_element_by_name(
    std::shared_ptr<Osc::Message> received_message) {
  if (received_message->format() == "is") {
    std::shared_ptr<Osc::Message> response =
        received_message->response("/element/by-name");

    int card_index = received_message->values[0].toInt();
    QString element_name = received_message->values[1].toString();

    std::shared_ptr<Alsa::Card> card = _cardmodel->card(card_index);

    if (card.get() == nullptr) {
      qWarning() << tr("Requested card index %1 is invalid").arg(card_index);
      return;
    }
    std::shared_ptr<Alsa::Ctl::Element> element =
        card->elementList()->getByName(element_name);

    if (element.get() == nullptr) {
      qWarning()
          << tr("Requested element with name '%1' not found").arg(element_name);
      return;
    }

    response->values.append(card_index);
    response->values.append(element_name);
    response->values.append(element->id());

    sendOscMessage(response);
  }
}
