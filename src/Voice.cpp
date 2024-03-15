#include <math.h>
#include "Voice.hpp"

namespace AyMidi {

    extern "C" {
#include "ayumi.h"
    }

    Voice::Voice(std::shared_ptr<SoundGenerator> sg, int index) :
        sg(sg),
        index(index)
    {
        setPan(0.5);
    }

    void Voice::mute() {
        enableEnvelope(false);
        enableTone(false);
        enableNoise(false);
        setLevel(0);
    }

    int Voice::freqToSquarePeriod(const double freq) const {
        return std::min((int)std::round(sg->getClockRate() / 16.0f / freq), 0x0FFF);
    }

    int Voice::freqToBuzzerPeriod(const double freq) const {
        return std::min((int)std::round(sg->getClockRate() / 256.0f / freq), 0xFFFF);
    }

    void Voice::setNoisePeriod(int period) {
        ayumi_set_noise(&*sg->getAyumi(), period);
    }

    void Voice::setEnvelopePeriod(int period) {
        ayumi_set_envelope(&*sg->getAyumi(), period);
    }

    void Voice::setEnvelopeShape(int shape) {
        ayumi_set_envelope_shape(&*sg->getAyumi(), shape);
    }

    void Voice::enableTone(bool enable) {
        toneOff = !enable;
        ayumi_set_mixer(&*sg->getAyumi(), index, toneOff, noiseOff, envelopeOn);
    }

    void Voice::enableNoise(bool enable) {
        noiseOff = !enable;
        ayumi_set_mixer(&*sg->getAyumi(), index, toneOff, noiseOff, envelopeOn);
    }

    void Voice::enableEnvelope(bool enable) {
        envelopeOn = enable;
        ayumi_set_mixer(&*sg->getAyumi(), index, toneOff, noiseOff, envelopeOn);
    }

    void Voice::setLevel(int level) {
        ayumi_set_volume(&*sg->getAyumi(), index, level);
    }

    void Voice::setTonePeriod(int period) {
        ayumi_set_tone(&*sg->getAyumi(), index, period);
    }

    void Voice::setPan(float pan) {
        ayumi_set_pan(&*sg->getAyumi(), index, pan, 1);
    }
}
