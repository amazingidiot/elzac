#include "alsa_ctl.h"

#include <QDebug>

Alsa::Ctl::ElementList::ElementList(std::shared_ptr<snd_ctl_t> snd_ctl) {
  qDebug() << tr("Opening card");

  this->_snd_ctl = snd_ctl;

  snd_ctl_elem_list_t *list;
  int count;

  // get the list from ALSA
  snd_ctl_elem_list_malloc(&list);
  snd_ctl_elem_list(_snd_ctl.get(), list);
  count = snd_ctl_elem_list_get_count(list);
  snd_ctl_elem_list_alloc_space(list, count);
  snd_ctl_elem_list(_snd_ctl.get(), list);

  qDebug() << tr("Loading elements...");

  for (int i = 0; i < count; i++) {
    unsigned int id = snd_ctl_elem_list_get_numid(list, i);

    Alsa::Ctl::Element *e = new Alsa::Ctl::Element(id, _snd_ctl);

    qDebug() << e->toString();

    this->_elements.append(std::shared_ptr<Element>(e));
  }

  qDebug() << tr("Creating sockets...");

  if (snd_ctl_poll_descriptors_count(_snd_ctl.get()) > 0) {
    qDebug() << tr("Subscribing to events");

    snd_ctl_subscribe_events(_snd_ctl.get(), 1);

    QList<pollfd> _pollfds;

    _pollfds.resize(snd_ctl_poll_descriptors_count(_snd_ctl.get()));

    snd_ctl_poll_descriptors(_snd_ctl.get(), _pollfds.data(), _pollfds.size());

    qDebug() << tr("Got %1 pollfds").arg(_pollfds.length());

    foreach (pollfd fd, _pollfds) {

      if (fd.fd != 0) {
        std::shared_ptr<QSocketNotifier> sn(
            new QSocketNotifier(fd.fd, QSocketNotifier::Read, this));
        sn->setEnabled(true);
        connect(sn.get(), &QSocketNotifier::activated, this,
                &Alsa::Ctl::ElementList::_socketActivated);
        _socket_notifiers.append(sn);
      }
    }
  } else {
    qCritical() << tr("CTL has no poll descriptors!");
  }
}

int Alsa::Ctl::ElementList::count() { return _elements.count(); }

std::shared_ptr<Alsa::Ctl::Element> Alsa::Ctl::ElementList::getByID(int id) {
  if ((id < 1) || (id > _elements.count())) {
    qWarning() << tr("Invalid element id requested: %1").arg(id);
    return std::shared_ptr<Ctl::Element>(nullptr);
  }

  std::shared_ptr<Element> element = _elements.at(id - 1);

  if (element->id() == id) {
    return element;
  }

  qWarning() << tr("Invalid element id requested: %1").arg(id);
  return std::shared_ptr<Ctl::Element>(nullptr);
}

std::shared_ptr<Alsa::Ctl::Element>
Alsa::Ctl::ElementList::getByName(QString name) {
  for (int i = 0; i < _elements.count(); i++) {
    if (_elements.at(i)->name() == name) {
      return _elements.at(i);
    }
  }

  return std::shared_ptr<Alsa::Ctl::Element>(nullptr);
}

Alsa::Ctl::Element::Element(int id, std::shared_ptr<snd_ctl_t> snd_ctl) {
  this->_snd_ctl = snd_ctl;

  snd_ctl_elem_info_t *_snd_ctl_elem_info_ptr;

  snd_ctl_elem_info_malloc(&_snd_ctl_elem_info_ptr);

  this->_snd_ctl_elem_info = std::shared_ptr<snd_ctl_elem_info_t>(
      _snd_ctl_elem_info_ptr, snd_ctl_elem_info_free);
  snd_ctl_elem_info_set_numid(_snd_ctl_elem_info.get(), id);
  snd_ctl_elem_info(_snd_ctl.get(), _snd_ctl_elem_info.get());

  _id = id;

  snd_ctl_elem_value_t *_snd_ctl_elem_value_ptr = nullptr;

  snd_ctl_elem_value_malloc(&_snd_ctl_elem_value_ptr);
  snd_ctl_elem_value_set_numid(_snd_ctl_elem_value_ptr, id);

  this->_snd_ctl_elem_value = std::shared_ptr<snd_ctl_elem_value_t>(
      _snd_ctl_elem_value_ptr, snd_ctl_elem_value_free);
}

QString Alsa::Ctl::Element::toString() {
  QString typeString(snd_ctl_elem_type_name(snd_ctl_elem_type_t(this->type())));

  return QString("Element %1: %2 (%3) - Count: %4")
      .arg(_id)
      .arg(name())
      .arg(typeString)
      .arg(channel_count());
}

QVariant Alsa::Ctl::Element::value(int channel) {

  snd_ctl_elem_read(_snd_ctl.get(), _snd_ctl_elem_value.get());

  switch (this->type()) {
  case 1: { // BOOLEAN
    return QVariant(
        snd_ctl_elem_value_get_boolean(_snd_ctl_elem_value.get(), channel));
  } break;
  case 2: { // INTEGER
    return QVariant(static_cast<int>(
        snd_ctl_elem_value_get_integer(_snd_ctl_elem_value.get(), channel)));
  } break;
  case 3: { // ENUMERATED
    return QVariant(
        snd_ctl_elem_value_get_enumerated(_snd_ctl_elem_value.get(), 0));
  } break;
  case 4: { // BYTES
    return QVariant();
  } break;
  case 5: { // IEC958
    return QVariant();
  }; break;
  case 6: { // INTEGER64
    return QVariant::fromValue(
        snd_ctl_elem_value_get_integer64(_snd_ctl_elem_value.get(), channel));
  }; break;
  default: {
    return QVariant();
  }
  }
}

void Alsa::Ctl::Element::setValue(QVariant value, int channel) {

  switch (this->type()) {
  case 1: { // BOOLEAN
    snd_ctl_elem_value_set_boolean(_snd_ctl_elem_value.get(), channel,
                                   value.toBool());
  } break;
  case 2: { // INTEGER
    qDebug() << QString("Int %1 to channel %2").arg(value.toInt()).arg(channel);
    snd_ctl_elem_value_set_integer(_snd_ctl_elem_value.get(), channel,
                                   value.toInt());
  } break;
  case 3: { // ENUMERATED
    snd_ctl_elem_value_set_enumerated(_snd_ctl_elem_value.get(), channel,
                                      value.toUInt());
  } break;
  case 4: { // BYTES

  } break;
  case 5: { // IEC958

  }; break;
  case 6: { // INTEGER64
    snd_ctl_elem_value_set_integer64(_snd_ctl_elem_value.get(), channel,
                                     value.toInt());
  }; break;
  }

  snd_ctl_elem_write(_snd_ctl.get(), _snd_ctl_elem_value.get());
}

QString Alsa::Ctl::Element::name() {
  return QString(snd_ctl_elem_info_get_name(_snd_ctl_elem_info.get()));
}

int Alsa::Ctl::Element::id() { return _id; }

int Alsa::Ctl::Element::card_index() {
  return snd_ctl_elem_info_get_device(_snd_ctl_elem_info.get());
}

int Alsa::Ctl::Element::type() {
  return snd_ctl_elem_info_get_type(_snd_ctl_elem_info.get());
}

int Alsa::Ctl::Element::channel_count() {
  return snd_ctl_elem_info_get_count(_snd_ctl_elem_info.get());
}

bool Alsa::Ctl::Element::isReadable() {
  return snd_ctl_elem_info_is_readable(_snd_ctl_elem_info.get());
}

bool Alsa::Ctl::Element::isWritable() {
  return snd_ctl_elem_info_is_writable(_snd_ctl_elem_info.get());
}

int Alsa::Ctl::Element::min() {
  return snd_ctl_elem_info_get_min(_snd_ctl_elem_info.get());
}

int Alsa::Ctl::Element::max() {
  return snd_ctl_elem_info_get_max(_snd_ctl_elem_info.get());
}

void Alsa::Ctl::ElementList::_socketActivated(QSocketDescriptor socket,
                                              QSocketNotifier::Type type) {
  qDebug() << tr("Socket was activated, type %1").arg(type);

  if (!socket.isValid()) {
    qCritical() << tr("Socket is no longer valid.");
    return;
  }

  snd_ctl_event_t *ptr = nullptr;
  unsigned int mask;
  int err, numid;

  snd_ctl_event_malloc(&ptr);
  std::shared_ptr<snd_ctl_event_t> event(ptr, snd_ctl_event_free);

  err = snd_ctl_read(_snd_ctl.get(), event.get());

  if (err == 0) {
    qCritical() << tr("Alsa triggered callback, but no data to read.");
    return;
  } else if (err < 0) {
    if (err == -ENODEV)
      return;

    qCritical() << tr("Alsa callback error: %1").arg(snd_strerror(err));
    return;
  } else if (snd_ctl_event_get_type(event.get()) != SND_CTL_EVENT_ELEM) {
    return;
  }

  numid = snd_ctl_event_elem_get_numid(event.get());

  std::shared_ptr<Element> elem = getByID(numid);

  mask = snd_ctl_event_elem_get_mask(event.get());

  if (mask & (SND_CTL_EVENT_MASK_VALUE | SND_CTL_EVENT_MASK_INFO)) {
    emit this->elementChanged(elem);

    qInfo() << tr("Element %1 (%2) changed!").arg(elem->id()).arg(elem->name());
  }
}
