#pragma once
#include <QAbstractListModel>
#include <QSocketNotifier>
#include <alsa/asoundlib.h>
#include <memory>
#include <vector>

namespace Alsa {
class Mixer : public QObject {
public:
    Mixer(snd_hctl_t* snd_hctl, QObject* parent = nullptr);
    ~Mixer();

    bool validateIndex(int index);
    int count();
    QString elementName(int index);
    int elementActive(int index);
    int elementHasPlayback(int index);
    int elementHasCapture(int index);
    int elementHasPlaybackVolume(int index);
    int elementHasCaptureVolume(int index);
    int elementHasPlaybackSwitch(int index);
    int elementHasCaptureSwitch(int index);
    int elementIsPlaybackEnum(int index);
    int elementIsCaptureEnum(int index);
    long elementPlaybackVolume(int index);
    long elementPlaybackVolumeMinimum(int index);
    long elementPlaybackVolumeMaximum(int index);
    long elementCaptureVolume(int index);
    long elementCaptureVolumeMinimum(int index);
    long elementCaptureVolumeMaximum(int index);
    long elementPlaybackdB(int index);
    long elementPlaybackdBMinimum(int index);
    long elementPlaybackdBMaximum(int index);
    long elementCapturedB(int index);
    long elementCapturedBMinimum(int index);
    long elementCapturedBMaximum(int index);
    int elementPlaybackSwitch(int index);
    int elementCaptureSwitch(int index);
    void elementEnumItems(int index);
    void elementEnumItem(int index);

private:
    snd_mixer_t* _snd_mixer = nullptr;
    snd_hctl_t* _snd_hctl = nullptr;
    QList<snd_mixer_elem_t*> _snd_mixer_elems;
    QList<pollfd> _pollfds;
    QList<std::shared_ptr<QSocketNotifier>> _socket_notifiers;

    static int _snd_mixer_callback(snd_mixer_t* mixer, unsigned int mask, snd_mixer_elem_t* elem);
    static int _snd_mixer_element_callback(snd_mixer_elem_t* elem, unsigned int mask);

private slots:
    void _mixer_event(int);

};
} // namespace alsa
