#include <math.h>
#include "Note.hpp"

namespace AyMidi {

    Note::Note(ChannelData* params, int key, int velocity) :
        params(params),
        key(key),
        velocity(velocity)
    {
        setup = true;
        valid = true;
        released = false;
    }

    void Note::setVoice(std::shared_ptr<Voice> voice) {
        this->voice = voice;
    }

    void Note::release() {
        released = true;
    }

    void Note::drop() {
        valid = false;
    }

    bool Note::isValid() {
        return valid;
    }

    bool Note::isReleased() {
        return released;
    }

    void Note::setPressure(int pressure) {
        this->pressure = pressure;
    }

    int Note::getLevel() const {
        return (int)(envelopeLevel * velocity * params->volume / 128.0f * 16.0f);
    }

    float Note::getNoteFreq(const double key) const {
        return 440.0f * pow(2, (key - 69) / 12.0f);
    }

    int Note::getSquareFreq() const {
        return getNoteFreq(key + envelopePitch + params->squareDetune + params->pitchBend * 12.0f);
    }

    int Note::getBuzzerFreq() const {
        float mult = params->buzzerWaveform == 2 || params->buzzerWaveform == 6 ? 2.0f : 1.0f;
        return getNoteFreq(key + envelopePitch + params->buzzerDetune + params->pitchBend * 12.0f);
    }

    void Note::updateEnvelope() {
        auto envelope = params->envelope;
        envelopePitch = 0.0f;
        if (released) {
            if (releaseCounter >= envelope.release) {
                valid = false;
            } else {
                if (releaseCounter == 0) {
                    releaseStartLevel = envelopeLevel;
                }
                envelopeLevel = releaseStartLevel * (1.0f - (float)releaseCounter / envelope.release);
                releaseCounter++;
            }
            return;
        }

        if (envelopeCounter >= (envelope.attack + envelope.hold + envelope.decay)) {
            if (envelope.sustain == 0.0f) {
                valid = false;
                return;
            }
            envelopeLevel = envelope.sustain;
            return;
        }

        auto counter = envelopeCounter;
        envelopeCounter++;
        if (envelope.attackPitch && counter < (envelope.attack + envelope.hold)) {
            envelopePitch = envelope.attackPitch * (1.0f - ((float)counter / (envelope.attack + envelope.hold)));
        }
        if (counter < envelope.attack) {
            envelopeLevel = (float)counter / envelope.attack;
            return;
        }
        counter -= envelope.attack;
        if (counter < envelope.hold) {
            envelopeLevel = 1.0f;
            return;
        }
        counter -= envelope.hold;
        if (counter < envelope.decay) {
            envelopeLevel = 1.0f + (envelope.sustain - 1.0f) * ((float)counter / envelope.decay);
            return;
        }
    }

    void Note::update(int updateRate) {
        voice->enableEnvelope(params->buzzer);
        voice->enableTone(params->square);
        if (params->buzzer) {
            if (setup) {
                voice->setEnvelopeShape(params->buzzerWaveform + 8);
                setup = false;
            }
            voice->setEnvelopeFreq(getBuzzerFreq());
        }
        if (params->square) {
            voice->setLevel(getLevel());
            voice->setToneFreq(getSquareFreq());
        }
        voice->enableNoise(params->noisePeriod > 0);
        if (params->noisePeriod > 0) {
            voice->setNoisePeriod(params->noisePeriod);
        }
        voice->setPan(params->pan);
    }
}
