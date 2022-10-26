#pragma once

#include <QDebug>
#include <QObject>
#include <alsa/asoundlib.h>
#include <memory>

#include "alsa_mixer_model.h"

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

    Mixer* mixer();

private:
    int _index = -1;
    QString _id;
    QString _driver;
    QString _name;
    QString _long_name;
    QString _mixer_name;
    QString _components;

    Mixer* _mixer = nullptr;

    snd_hctl_t* _snd_hctl = nullptr;
};
} // namespace alsa
