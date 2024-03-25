#include <math.h>
#include "Note.hpp"

namespace AyMidi {

    Note::Note(ChannelData* params, int key, int velocity, int channelId) :
        params(params),
        key(key),
        velocity(velocity),
        channelId(channelId),
        startKey(0)
    {
        setup = true;
        valid = true;
        released = false;
        if (params->portamentoControl > 0) {
            startKey = params->portamentoControl;
            params->portamentoControl = 0;
        }
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

    void Note::setStartKey(int key) {
        if (startKey == 0) {
            startKey = key;
        }
    }

    int Note::getLevel() const {
        return (int)(envelopeLevel * velocity * params->volume / 128.0f * 16.0f);
    }

    float Note::getNoteFreq(const double key) const {
        return 440.0f * pow(2, (key - 69) / 12.0f);
    }

    int Note::getSquareFreq(int updateRate) const {
        auto vibratoPitch = 0.0f;
        if (params->vibratoDepth > 0 && params->vibratoRate > 0 && (10.0f * timeCounter / updateRate) >= params->vibratoDelay) {
            vibratoPitch = params->vibratoDepth * std::sin(params->vibratoRate * timeCounter / updateRate * 2.0f * M_PI);
        }
        auto portamentoPitch = 0.0f;
        if (params->portamento && params->portamentoTime > 0 && (10.0f * timeCounter / updateRate < params->portamentoTime) && startKey != 0) {
            portamentoPitch = startKey + (key - startKey) * 10.0f * timeCounter / updateRate / params->portamentoTime - key;
        }
        return getNoteFreq(key + envelopePitch + vibratoPitch + portamentoPitch + params->squareDetune + params->pitchBend * 12.0f);
    }

    int Note::getBuzzerFreq(int updateRate) const {
        auto vibratoPitch = 0.0f;
        if (params->vibratoDepth > 0 && params->vibratoRate > 0 && (10.0f * timeCounter / updateRate) >= params->vibratoDelay) {
            vibratoPitch = params->vibratoDepth * std::sin(params->vibratoRate * timeCounter / updateRate * 2.0f * M_PI);
        }
        auto portamentoPitch = 0.0f;
        if (params->portamento && params->portamentoTime > 0 && (10.0f * timeCounter / updateRate < params->portamentoTime) && startKey != 0) {
            portamentoPitch = startKey + (key - startKey) * 10.0f * timeCounter / updateRate / params->portamentoTime - key;
        }
        float mult = params->buzzerWaveform == 2 || params->buzzerWaveform == 6 ? 2.0f : 1.0f;
        return getNoteFreq(key + envelopePitch + vibratoPitch + portamentoPitch + params->buzzerDetune + params->pitchBend * 12.0f);
    }

    void Note::updateEnvelope() {
        auto envelope = params->envelope;
        auto envelopeCounter = timeCounter;
        timeCounter++;
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

        if (envelope.attackPitch && envelopeCounter < (envelope.attack + envelope.hold)) {
            envelopePitch = envelope.attackPitch * (1.0f - ((float)envelopeCounter / (envelope.attack + envelope.hold)));
        }
        if (envelopeCounter < envelope.attack) {
            envelopeLevel = (float)envelopeCounter / envelope.attack;
            return;
        }
        envelopeCounter -= envelope.attack;
        if (envelopeCounter < envelope.hold) {
            envelopeLevel = 1.0f;
            return;
        }
        envelopeCounter -= envelope.hold;
        if (envelopeCounter < envelope.decay) {
            envelopeLevel = 1.0f + (envelope.sustain - 1.0f) * ((float)envelopeCounter / envelope.decay);
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
            voice->setEnvelopeFreq(getBuzzerFreq(updateRate));
        }
        if (params->square) {
            voice->setLevel(getLevel());
            voice->setToneFreq(getSquareFreq(updateRate));
        }
        voice->enableNoise(params->noisePeriod > 0);
        if (params->noisePeriod > 0) {
            voice->setNoisePeriod(params->noisePeriod);
        }
        voice->setPan(params->pan);
    }
}
