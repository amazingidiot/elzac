#include "alsa_mixer_model.h"

Alsa::Mixer::Mixer(snd_hctl_t* snd_hctl, QObject* parent)
{
    qDebug() << tr("Opening mixer");

    _snd_hctl = snd_hctl;

    int result = snd_mixer_open(&_snd_mixer, 0);

    if (result < 0) {
        qCritical() << tr("Failed to open mixer: %1").arg(result);
        return;
    }

    qDebug() << tr("Opened mixer");

    result = snd_mixer_selem_register(_snd_mixer, NULL, NULL);

    if (result < 0) {
        qCritical() << tr("Failed to register selems: %1").arg(result);
        return;
    }

    qDebug() << tr("Registered selems");

    result = snd_mixer_attach_hctl(_snd_mixer, _snd_hctl);

    if (result < 0) {
        qCritical() << tr("Failed to attach mixer: %1").arg(result);
        return;
    }

    qDebug() << tr("Attached mixer");

    result = snd_mixer_load(_snd_mixer);

    if (result < 0) {
        qCritical() << tr("Failed to load mixer elements: %1").arg(result);
        return;
    }

    qDebug() << tr("Loaded mixer elements");

    int element_count = snd_mixer_get_count(_snd_mixer);

    switch (element_count) {
        case 0: {
            qWarning() << tr("Mixer has no elements");
        } break;
        case 1: {
            qInfo() << tr("Mixer has 1 element");

            _snd_mixer_elems.append(snd_mixer_first_elem(_snd_mixer));
        } break;
        default: {
            qInfo() << tr("Mixer has %1 elements").arg(element_count);

            snd_mixer_elem_t* current_element = snd_mixer_first_elem(_snd_mixer);
            snd_mixer_elem_t* last_element = snd_mixer_last_elem(_snd_mixer);

            while (current_element != last_element) {
                _snd_mixer_elems.append(current_element);
                current_element = snd_mixer_elem_next(current_element);
            }
        }
    }

    snd_mixer_set_callback(_snd_mixer, &Mixer::_snd_mixer_callback);
    snd_mixer_set_callback_private(_snd_mixer, this);


    if (snd_mixer_poll_descriptors_count(_snd_mixer) > 0) {
        _pollfds.resize(snd_mixer_poll_descriptors_count(_snd_mixer));

        snd_mixer_poll_descriptors(_snd_mixer, _pollfds.data(), _pollfds.size());

        qDebug() << tr("Got %1 pollfds").arg(_pollfds.length());

        foreach (pollfd fd, _pollfds) {
            if (fd.fd != 0) {
                std::shared_ptr<QSocketNotifier> sn (new QSocketNotifier(fd.fd, QSocketNotifier::Read, this));
                connect(sn.get(), &QSocketNotifier::activated, this, &Mixer::_mixer_event);
                _socket_notifiers.append(sn);
            }
        }
    }
}

Alsa::Mixer::~Mixer()
{
    qDebug() << tr("Closing mixer");

    if (_snd_mixer != nullptr) {
        snd_mixer_set_callback(_snd_mixer, nullptr);
        snd_mixer_detach_hctl(_snd_mixer, _snd_hctl);
        qDebug() << tr("Detached hctl from mixer");

        snd_mixer_close(_snd_mixer);
    }
}

int Alsa::Mixer::count()
{
    return _snd_mixer_elems.count();
}

QString Alsa::Mixer::elementName(int index)
{
    if (!validateIndex(index)) {
        return QString();
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    return QString(snd_mixer_selem_get_name(element));
}

int Alsa::Mixer::elementActive(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    return snd_mixer_selem_is_active(element);
}

int Alsa::Mixer::elementHasPlayback(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    return snd_mixer_selem_has_playback_channel(element, SND_MIXER_SCHN_MONO);
}

int Alsa::Mixer::elementHasCapture(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    return snd_mixer_selem_has_capture_channel(element, SND_MIXER_SCHN_MONO);
}

int Alsa::Mixer::elementHasPlaybackVolume(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    return snd_mixer_selem_has_playback_volume(element);
}

int Alsa::Mixer::elementHasCaptureVolume(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    return snd_mixer_selem_has_capture_volume(element);
}

int Alsa::Mixer::elementHasPlaybackSwitch(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    return snd_mixer_selem_has_playback_switch(element);
}

int Alsa::Mixer::elementHasCaptureSwitch(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    return snd_mixer_selem_has_capture_switch(element);
}

int Alsa::Mixer::elementIsPlaybackEnum(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    return snd_mixer_selem_is_enum_playback(element);
}

int Alsa::Mixer::elementIsCaptureEnum(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    return snd_mixer_selem_is_enum_capture(element);
}

long Alsa::Mixer::elementPlaybackVolume(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    long value = 0;

    if (snd_mixer_selem_get_playback_volume(element, SND_MIXER_SCHN_MONO, &value) == 0) {
        qDebug() << tr("Element %1 volume: %2").arg(index).arg(value);
        return value;
    } else {
        qWarning() << tr("Could not read playback volume for element %1").arg(index);
        return 0;
    }
}

long Alsa::Mixer::elementPlaybackVolumeMinimum(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    long min, max = 0;

    if (snd_mixer_selem_get_playback_volume_range(element, &min, &max) == 0) {
        qDebug() << tr("Element %1 min: %2").arg(index).arg(min);
        return min;
    } else {
        qWarning() << tr("Could not read playback range minimum for element %1").arg(index);
        return 0;
    }
}

long Alsa::Mixer::elementPlaybackVolumeMaximum(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    long min, max = 0;

    if (snd_mixer_selem_get_playback_volume_range(element, &min, &max) == 0) {
        qDebug() << tr("Element %1 max: %2").arg(index).arg(max);
        return max;
    } else {
        qWarning() << tr("Could not read playback range maximum for element %1").arg(index);
        return 0;
    }
}

long Alsa::Mixer::elementCaptureVolume(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    long value = 0;

    if (snd_mixer_selem_get_capture_volume(element, SND_MIXER_SCHN_MONO, &value) == 0) {
        qDebug() << tr("Element %1 volume: %2").arg(index).arg(value);
        return value;
    } else {
        qWarning() << tr("Could not read playback volume for element %1").arg(index);
        return 0;
    }
}

long Alsa::Mixer::elementCaptureVolumeMinimum(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    long min, max = 0;

    if (snd_mixer_selem_get_capture_volume_range(element, &min, &max) == 0) {
        qDebug() << tr("Element %1 min: %2").arg(index).arg(min);
        return min;
    } else {
        qWarning() << tr("Could not read playback range maximum for element %1").arg(index);
        return 0;
    }
}

long Alsa::Mixer::elementCaptureVolumeMaximum(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    long min, max = 0;

    if (snd_mixer_selem_get_capture_volume_range(element, &min, &max) == 0) {
        qDebug() << tr("Element %1 max: %2").arg(index).arg(max);
        return max;
    } else {
        qWarning() << tr("Could not read playback range maximum for element %1").arg(index);
        return 0;
    }
}

long Alsa::Mixer::elementPlaybackdB(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    long value = 0;

    if (snd_mixer_selem_get_playback_dB(element, SND_MIXER_SCHN_MONO, &value) == 0) {
        return value;
    } else {
        qWarning() << tr("Could not read playback dB for element %1").arg(index);
        return 0;
    }
}

long Alsa::Mixer::elementPlaybackdBMinimum(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    long min, max = 0;

    if (snd_mixer_selem_get_playback_dB_range(element, &min, &max) == 0) {
        return max;
    } else {
        qWarning() << tr("Could not read playback dB range maximum for element %1").arg(index);
        return 0;
    }
}

long Alsa::Mixer::elementPlaybackdBMaximum(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    long min, max = 0;

    if (snd_mixer_selem_get_playback_dB_range(element, &min, &max) == 0) {
        return max;
    } else {
        qWarning() << tr("Could not read playback dB range maximum for element %1").arg(index);
        return 0;
    }
}

long Alsa::Mixer::elementCapturedB(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    long value = 0;

    if (snd_mixer_selem_get_capture_dB(element, SND_MIXER_SCHN_MONO, &value) == 0) {
        return value;
    } else {
        qWarning() << tr("Could not read capture dB for element %1").arg(index);
        return 0;
    }
}

long Alsa::Mixer::elementCapturedBMinimum(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    long min, max = 0;

    if (snd_mixer_selem_get_capture_dB_range(element, &min, &max) == 0) {
        return max;
    } else {
        qWarning() << tr("Could not read cature dB range maximum for element %1").arg(index);
        return 0;
    }
}

long Alsa::Mixer::elementCapturedBMaximum(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    long min, max = 0;

    if (snd_mixer_selem_get_capture_dB_range(element, &min, &max) == 0) {
        return max;
    } else {
        qWarning() << tr("Could not read capture dB range maximum for element %1").arg(index);
        return 0;
    }
}

int Alsa::Mixer::elementPlaybackSwitch(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    int value = 0;

    if (snd_mixer_selem_get_playback_switch(element, SND_MIXER_SCHN_MONO, &value) == 0) {
        return value;
    } else {
        qWarning() << tr("Could not read capture dB for element %1").arg(index);
        return 0;
    }
}

int Alsa::Mixer::elementCaptureSwitch(int index)
{
    if (!validateIndex(index)) {
        return 0;
    }

    snd_mixer_elem_t* element = _snd_mixer_elems[index];

    int value = 0;

    if (snd_mixer_selem_get_capture_switch(element, SND_MIXER_SCHN_MONO, &value) == 0) {
        return value;
    } else {
        qWarning() << tr("Could not read capture dB for element %1").arg(index);
        return 0;
    }
}

void Alsa::Mixer::elementEnumItems(int index)
{

}

void Alsa::Mixer::elementEnumItem(int index)
{

}

int Alsa::Mixer::_snd_mixer_callback(snd_mixer_t *mixer, unsigned int mask, snd_mixer_elem_t *elem)
{
    qDebug() << "Mixer callback activated!";

    return 0;
}

int Alsa::Mixer::_snd_mixer_element_callback(snd_mixer_elem_t *elem, unsigned int mask)
{
    qDebug() << "Element callback";

    return 0;
}

void Alsa::Mixer::_mixer_event(int)
{
    int event_count = snd_mixer_handle_events (_snd_mixer);
    if ( event_count < 0 ) {
        // Error. Disable socket notifiers
        qWarning() << tr("Error while handling events");
        return;
    }

    qDebug() << tr("%1 Mixer events caught").arg(event_count);
}

bool Alsa::Mixer::validateIndex(int index)
{
    if (index < 0) {
        qWarning() << tr("Requested element index %1 is lower than 0");
        return false;
    }
    if (index >= _snd_mixer_elems.count()) {
        qWarning() << tr("Requested element index %1 is greater than the available elements");
        return false;
    }

    return true;
}
