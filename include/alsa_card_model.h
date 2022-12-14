#ifndef ALSA_CARD_MODEL_H
#define ALSA_CARD_MODEL_H

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
  explicit CardModel(QObject *parent = nullptr);
  ~CardModel();

  QList<int> cards();
  std::shared_ptr<Alsa::Card> card(int index);
  int cardCount();

signals:
  void cardsChanged();
  void cardCreated(std::shared_ptr<Alsa::Card> card);
  void cardRemoved(int index);

private:
  QHash<int, std::shared_ptr<Alsa::Card>> _cards;

  std::shared_ptr<udev> _udev;
  std::shared_ptr<udev_monitor> _udev_monitor;

  QSocketNotifier *_udev_socket;

public slots:
  void updateCards();
};
} // namespace Alsa
#endif
