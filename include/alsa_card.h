#ifndef ALSA_CARD_H
#define ALSA_CARD_H

#include <QDebug>
#include <QObject>
#include <alsa/asoundlib.h>
#include <memory>

#include "alsa_ctl.h"

namespace Alsa {
class Card : QObject {
  Q_OBJECT

public:
  Card(int index);
  ~Card();

  int index();
  QString id();
  QString driver();
  QString name();
  QString longName();
  QString mixerName();
  QString components();

  QString toString();

  std::shared_ptr<Alsa::Ctl::ElementList> elementList();

private:
  int _index = -1;
  QString _id;
  QString _driver;
  QString _name;
  QString _long_name;
  QString _mixer_name;
  QString _components;

  std::shared_ptr<Alsa::Ctl::ElementList> _ctl_elements;
  std::shared_ptr<snd_ctl_t> _snd_ctl;
};
} // namespace Alsa

#endif