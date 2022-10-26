#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QSocketNotifier>
#include <QVariant>
#include <libudev.h>
#include <memory>

#include "alsa_card.h"

namespace Alsa {
class CardModel : public QObject {
    Q_OBJECT
public:
    explicit CardModel(QObject* parent = nullptr);
    ~CardModel();

    QList<int> cards();
    std::shared_ptr<Alsa::Card> card(int index);
    int cardCount();

signals:
    void cardAdded(int index);
    void cardRemoved(int index);

private:
    QHash<int, std::shared_ptr<Alsa::Card>> _cards;

    std::unique_ptr<udev, decltype(&udev_unref)> _udev;
    std::unique_ptr<udev_monitor, decltype(&udev_monitor_unref)> _udev_monitor;

    QSocketNotifier* _udev_socket;

private slots:
    void updateCards();
};
} // namespace alsa
