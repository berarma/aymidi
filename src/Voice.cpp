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

    void Voice::setNoisePeriod(int period) {
        sg->setNoisePeriod(period);
    }

    void Voice::setEnvelopePeriod(int period) {
        sg->setEnvelopePeriod(period);
    }

    void Voice::setEnvelopeFreq(int freq) {
        sg->setEnvelopeFreq(freq);
    }

    void Voice::setEnvelopeShape(int shape) {
        sg->setEnvelopeShape(shape);
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

    void Voice::setToneFreq(int freq) {
        ayumi_set_tone(&*sg->getAyumi(), index, sg->freqToSquarePeriod(freq));
    }

    void Voice::setPan(float pan) {
        ayumi_set_pan(&*sg->getAyumi(), index, pan, 1);
    }
}
