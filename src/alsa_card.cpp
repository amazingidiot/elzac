#include "alsa_card.h"

#include <QDebug>
#include <memory>

#include "alsa/asoundlib.h"

Alsa::Card::Card(int index) {
  snd_ctl_t* _snd_ctl_ptr = nullptr;

  int result_snd_ctl_open = snd_ctl_open(
      &_snd_ctl_ptr, QString("hw:%1").arg(index).toLocal8Bit().constData(),
      SND_CTL_NONBLOCK);

  if (result_snd_ctl_open < 0) {
    qCritical() << tr("Failed to open hctl for device hw:%1: %2")
                       .arg(index)
                       .arg(snd_strerror(result_snd_ctl_open));
    return;
  }

  _snd_ctl = std::shared_ptr<snd_ctl_t>(_snd_ctl_ptr, snd_ctl_close);

  snd_ctl_card_info_t* snd_card_info = nullptr;

  snd_ctl_card_info_alloca(&snd_card_info);

  int result_snd_ctl_card_info =
      snd_ctl_card_info(_snd_ctl.get(), snd_card_info);

  if (result_snd_ctl_card_info < 0) {
    qCritical() << tr("Failed to read card information for card %1").arg(index);
    return;
  }

  this->_index = index;
  this->_id = QString(snd_ctl_card_info_get_id(snd_card_info));
  this->_name = QString(snd_ctl_card_info_get_name(snd_card_info));
  this->_long_name = QString(snd_ctl_card_info_get_longname(snd_card_info));
  this->_mixer_name = QString(snd_ctl_card_info_get_mixername(snd_card_info));
  this->_components = QString(snd_ctl_card_info_get_components(snd_card_info));
  this->_driver = QString(snd_ctl_card_info_get_driver(snd_card_info));

  qDebug() << tr("Card index: %1").arg(_index);
  qDebug() << tr("ID: %1").arg(_id);
  qDebug() << tr("Name: %1").arg(_name);
  qDebug() << tr("Longname: %1").arg(_long_name);
  qDebug() << tr("Mixername: %1").arg(_mixer_name);
  qDebug() << tr("Driver: %1").arg(_driver);
  qDebug() << tr("Components: %1").arg(_components);

  //_mixer = std::shared_ptr<Mixer>(new Alsa::Mixer(_snd_ctl));
  _ctl_elements = std::shared_ptr<Alsa::Ctl::ElementList>(
      new Alsa::Ctl::ElementList(_snd_ctl));
}

Alsa::Card::~Card() { qDebug() << tr("Removing card %1").arg(this->index()); }

int Alsa::Card::index() { return _index; }

QString Alsa::Card::id() { return this->_id; }

QString Alsa::Card::driver() { return this->_driver; }

QString Alsa::Card::name() { return this->_name; }

QString Alsa::Card::longName() { return this->_long_name; }

QString Alsa::Card::mixerName() { return this->_mixer_name; }

QString Alsa::Card::components() { return this->_components; }

std::shared_ptr<Alsa::Ctl::ElementList> Alsa::Card::elementList() {
  return this->_ctl_elements;
}
