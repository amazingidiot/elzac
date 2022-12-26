#ifndef ALSA_CTL_H
#define ALSA_CTL_H

#include <QList>
#include <QObject>
#include <QSocketNotifier>
#include <QVariant>

#include <alsa/asoundlib.h>
#include <memory>

namespace Alsa {
namespace Ctl {
class Element {
public:
  Element(int id, std::shared_ptr<snd_ctl_t> snd_ctl);

  QString toString();

  QString name();
  int id();
  int card_index();
  int type();
  int channel_count();
  QVariant value(int channel = 0);
  void setValue(QVariant value, int channel = 0);
  bool isReadable();
  bool isWritable();
  int min();
  int max();
  QVariant valuedB(int channel = 0);
  void setValuedB(QVariant value, int channel = 0);
  int mindB();
  int maxdB();
  QStringList enum_list();
  QString enum_name(int index);

private:
  std::shared_ptr<snd_ctl_t> _snd_ctl;
  std::shared_ptr<snd_ctl_elem_info_t> _snd_ctl_elem_info;
  std::shared_ptr<snd_ctl_elem_value_t> _snd_ctl_elem_value;

  int _id;
  int _card_index;
};

class ElementList : public QObject {
  Q_OBJECT

public:
  ElementList(std::shared_ptr<snd_ctl_t> snd_ctl);

  int count();

  std::shared_ptr<Element> getByID(int id);
  std::shared_ptr<Element> getByName(QString name);

signals:
  void elementChanged(std::shared_ptr<Alsa::Ctl::Element> element);

private:
  std::shared_ptr<snd_ctl_t> _snd_ctl;

  QList<std::shared_ptr<Element>> _elements;
  QList<std::shared_ptr<QSocketNotifier>> _socket_notifiers;

private slots:
  void _socketActivated(QSocketDescriptor socket, QSocketNotifier::Type type);
};
} // namespace Ctl
} // namespace Alsa
#endif
