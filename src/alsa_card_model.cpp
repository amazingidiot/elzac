#include "alsa_card_model.h"

#include <alsa/asoundlib.h>

#include <QDebug>

QList<int>
get_alsa_cards()
{
    QList<int> cards;

    int card_index = -1;

    while (true) {
        int card_next_error = snd_card_next(&card_index);

        if (card_next_error) {
            qWarning() << Alsa::CardModel::tr("Error reading all cards: %1").arg(card_next_error);
            break;
        }

        if (card_index == -1) {
            break;
        }

        qInfo() << Alsa::CardModel::tr("Found card %1").arg(card_index);

        cards.append(card_index);
    };

    return cards;
}

Alsa::CardModel::CardModel(QObject* parent)
    : _udev(nullptr, udev_unref)
    , _udev_monitor(nullptr, udev_monitor_unref)
{
    this->setParent(parent);

    udev* udev_ptr = udev_new();

    if (udev_ptr == NULL) {
        qCritical() << tr("Failed to initialize udev");
        return;
    }

    qDebug() << tr("Initialized udev");

    _udev.reset(udev_ptr);

    udev_monitor* udev_monitor_ptr = udev_monitor_new_from_netlink(_udev.get(), "udev");

    if (udev_monitor_ptr == NULL) {
        qCritical() << tr("Failed to initialize udev monitor");
        _udev.reset(nullptr);
        return;
    }

    qDebug() << tr("Initialized udev monitor");

    _udev_monitor.reset(udev_monitor_ptr);

    udev_monitor_filter_add_match_subsystem_devtype(_udev_monitor.get(), "sound", 0);
    udev_monitor_enable_receiving(_udev_monitor.get());

    _udev_socket = new QSocketNotifier(udev_monitor_get_fd(_udev_monitor.get()), QSocketNotifier::Read, this);

    connect(_udev_socket, &QSocketNotifier::activated, this, &Alsa::CardModel::updateCards);

    QList<int> cards = get_alsa_cards();

    foreach (int card, cards) {
        this->_cards[card] =
            std::shared_ptr<Alsa::Card>(new Alsa::Card(card));
    }
}

Alsa::CardModel::~CardModel()
{
    _udev_monitor.reset(nullptr);
    _udev.reset(nullptr);
}

QList<int> Alsa::CardModel::cards()
{
    return _cards.keys();
}

std::shared_ptr<Alsa::Card> Alsa::CardModel::card(int index)
{
    if (_cards.contains(index)) {
        return _cards[index];
    } else {
        return std::shared_ptr<Alsa::Card>(nullptr);
    }
}

int Alsa::CardModel::cardCount()
{
    return _cards.count();
}

void Alsa::CardModel::updateCards()
{
    qDebug() << tr("Card update triggered");

    bool any_change = false;

    while (true) {
        udev_device* dev(udev_monitor_receive_device(_udev_monitor.get()));

        if (dev != 0) {
            QString devnode(udev_device_get_devnode(dev));
            QString subsystem(udev_device_get_subsystem(dev));
            QString devtype(udev_device_get_devtype(dev));
            QString action(udev_device_get_action(dev));

            qInfo() << tr("Device change: Node: %1 - Subsystem: %2 - Devtype: %3 - Action: %4").arg(devnode).arg(subsystem).arg(devtype).arg(action);

            any_change = true;

            udev_device_unref(dev);
        } else {
            break;
        }
    }

    if (any_change) {
        qDebug() << tr("Updating card list");

        QList<int> cards = get_alsa_cards();

        foreach (int card_index, cards) {
            if (!_cards.contains(card_index)) {
                _cards[card_index] = std::shared_ptr<Alsa::Card>(new Alsa::Card(card_index));
            }
        }

        foreach (int card_index, _cards.keys()) {
            if (!cards.contains(card_index)) {
                _cards.remove(card_index);
            }
        }
    }
}
