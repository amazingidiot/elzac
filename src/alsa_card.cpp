#include "alsa_card.h"

#include <QDebug>
#include <memory>

#include "alsa/asoundlib.h"

Alsa::Card::Card(int index)
{
    int result_snd_hctl_open = snd_hctl_open(
        &_snd_hctl, QString("hw:%1").arg(index).toLocal8Bit().constData(),
        SND_CTL_NONBLOCK);

    if (result_snd_hctl_open < 0) {
        qCritical() << tr("Failed to open hctl for device hw:%1").arg(index);
        return;
    }

    snd_ctl_t* snd_card_ctl = snd_hctl_ctl(_snd_hctl);

    snd_ctl_card_info_t* snd_card_info = nullptr;

    snd_ctl_card_info_alloca(&snd_card_info);

    int result_snd_ctl_card_info
        = snd_ctl_card_info(snd_card_ctl, snd_card_info);

    if (result_snd_ctl_card_info < 0) {
        qCritical()
            << tr("Failed to read card information for card %1").arg(index);
        return;
    }

    this->_index = index;
    this->_id = QString(snd_ctl_card_info_get_id(snd_card_info));
    this->_name = QString(snd_ctl_card_info_get_name(snd_card_info));
    this->_long_name = QString(snd_ctl_card_info_get_longname(snd_card_info));
    this->_mixer_name
        = QString(snd_ctl_card_info_get_mixername(snd_card_info));
    this->_components
        = QString(snd_ctl_card_info_get_components(snd_card_info));
    this->_driver = QString(snd_ctl_card_info_get_driver(snd_card_info));

    qDebug() << tr("Card index: %1").arg(_index);
    qDebug() << tr("ID: %1").arg(_id);
    qDebug() << tr("Name: %1").arg(_name);
    qDebug() << tr("Longname: %1").arg(_long_name);
    qDebug() << tr("Mixername: %1").arg(_mixer_name);
    qDebug() << tr("Driver: %1").arg(_driver);
    qDebug() << tr("Components: %1").arg(_components);

    _mixer = new Alsa::Mixer(_snd_hctl, this);
}

Alsa::Card::~Card()
{
    qDebug() << tr("Removing card %1").arg(this->index());

    if (_mixer != nullptr) {
        delete _mixer;
        qDebug() << tr("Closed mixer");
    }

    if (_snd_hctl != nullptr) {
        snd_hctl_close(_snd_hctl);
        qDebug() << tr("Closed snd_hctl");
    }
}

int Alsa::Card::index()
{
    return _index;
}

QString
Alsa::Card::id()
{
    return this->_id;
}

QString
Alsa::Card::driver()
{
    return this->_driver;
}

QString
Alsa::Card::name()
{
    return this->_name;
}

QString
Alsa::Card::longName()
{
    return this->_long_name;
}

QString
Alsa::Card::mixerName()
{
    return this->_mixer_name;
}

QString
Alsa::Card::components()
{
    return this->_components;
}

Alsa::Mixer *Alsa::Card::mixer()
{
    return this->_mixer;
}
