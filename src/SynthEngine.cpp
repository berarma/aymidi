#include <algorithm>
#include <cmath>
#include <limits>
#include "DistrhoUtils.hpp"
#include "SynthEngine.hpp"

namespace AyMidi {

    SynthEngine::SynthEngine(std::shared_ptr<SoundGenerator> sg) : sg(sg) {
        for (int i = 0; i < 16; i++) {
            channels[i] = std::make_unique<Channel>(i);
        }
        for (int i = 0; i < 3; i++) {
            voices[i] = nullptr;
            voicePool.push_back(i);
        }

        setUpdateRate(50);
    }

    void SynthEngine::setUpdateRate(int rate) {
        updatePeriod = std::round((float)sg->sampleRate / rate);
        updateCounter = 0;
    }

    void SynthEngine::midiSend(const uint8_t* message) {
        const uint8_t status = message[0];
        const int index = status & 0xF;

        Channel* channel = &*channels[index];

        switch (getMidiMsgStatus(message)) {
            case MIDI_MSG_NOTE_OFF:
                channel->cmdNoteOff(message[1], message[2]);
                break;
            case MIDI_MSG_NOTE_ON:
                {
                    auto voice = channel->cmdNoteOn(message[1], message[2]);
                    auto slotIndex = voicePool.back();
                    if (voices[slotIndex] != nullptr) {
                        channel->cmdNoteOff(voices[slotIndex]->note, 0);
                    }
                    voices[slotIndex] = voice;
                    std::rotate(voicePool.begin(), voicePool.end() - 1, voicePool.end());
                }
                break;
            case MIDI_MSG_KEY_PRESSURE:
                channel->cmdKeyPressure(message[1], message[2]);
                break;
            case MIDI_MSG_CHANNEL_PRESSURE:
                channel->pressure = message[1] / 127.0f;
                break;
            case MIDI_MSG_PITCH_BEND:
                channel->pitchBend = signedFloat(message[1] + (message[2] << 7), 14);
                break;
            case MIDI_MSG_PGM_CHANGE:
                channel->program = std::min<uint8_t>(39, message[1]);
                break;
            case MIDI_MSG_RESET:
                channel->cmdReset();
                break;
            case MIDI_MSG_CONTROL:
                switch (message[1]) {
                    case MIDI_CTL_RESET_CONTROLLERS:
                        channel->cmdResetCC();
                        break;
                    case MIDI_CTL_ALL_SOUNDS_OFF:
                    case MIDI_CTL_ALL_NOTES_OFF:
                        channel->cmdAllNotesOff();
                        break;
                    case MIDI_CTL_MSB_MODWHEEL:
                        channel->modWheel = signedFloat(message[2], 7);
                        break;
                    case MIDI_CTL_MSB_PAN:
                        channel->pan = unsignedFloat(message[2], 7);
                        break;
                    case MIDI_CTL_MSB_MAIN_VOLUME:
                        channel->volume = message[2] / 127.0;
                        break;
                    /* AY/YM Effects */
                    case MIDI_CTL_AY_NOISE_PERIOD:
                        channels[index]->noisePeriod = spreadInt(message[2], 7, 32);
                        break;
                    case MIDI_CTL_AY_BUZZER_WAVEFORM:
                        channels[index]->buzzerWaveform = spreadInt(message[2], 7, 8);
                        break;
                    case MIDI_CTL_AY_MIX_BOTH:
                        channels[index]->mixBoth = message[2] >= 64;
                        break;
                    case MIDI_CTL_AY_BUZ_SQR_RATIO:
                        channels[index]->multRatio = spreadInt(message[2], 7, 8);
                        break;
                    case MIDI_CTL_AY_BUZ_SQR_DETUNE:
                        channels[index]->multDetune = signedFloat(message[2], 7) * 63;
                        break;
                    case MIDI_CTL_AY_ATTACK_PITCH:
                        channels[index]->attackPitch = signedFloat(message[2], 7) * 12;
                        break;
                    case MIDI_CTL_AY_ATTACK:
                        channels[index]->attack = message[2];
                        break;
                    case MIDI_CTL_AY_HOLD:
                        channels[index]->hold = message[2];
                        break;
                    case MIDI_CTL_AY_DECAY:
                        channels[index]->decay = message[2];
                        break;
                    case MIDI_CTL_AY_SUSTAIN:
                        channels[index]->sustain = message[2] / 127.0f;
                        break;
                    case MIDI_CTL_AY_RELEASE:
                        channels[index]->release = message[2];
                        break;
                    case MIDI_CTL_AY_ARPEGGIO_SPEED:
                        channels[index]->arpeggioSpeed = signedFloat(message[2], 7) * 63;
                        break;
                    default:
                        break;
                }
        }
    }

    void SynthEngine::synch() {
        for (int index = 0; index < 3; index++) {
            const auto& voice = voices[index];
            if (voice == nullptr) {
                continue;
            }
            const auto& channel = channels[voice->channelId];
            const bool buzzer = channel->program == 1 || channel->mixBoth;
            const bool square = channel->program == 0 || channel->mixBoth;
            const bool baseBuzzer = channel->program == 1;
            updateEnvelope(voice, channel);
            if (voice->remove) {
                sg->enableEnvelope(index, false);
                sg->enableTone(index, false);
                sg->enableNoise(index, false);
                sg->setLevel(index, 0);
                auto poolIndex = std::find(voicePool.begin(), voicePool.end(), index);
                std::rotate(poolIndex, poolIndex + 1, voicePool.end());
                voices[index] = nullptr;
                continue;
            }
            sg->enableEnvelope(index, buzzer);
            sg->enableTone(index, square);
            if (buzzer && voice->buzzerWaveform != channel->buzzerWaveform) {
                sg->setEnvelopeShape(channel->buzzerWaveform + 8);
                sg->syncTone(index);
                voice->buzzerWaveform = channel->buzzerWaveform;
            }
            int tonePeriod;
            int buzzerPeriod;
            if (buzzer && baseBuzzer) {
                buzzerPeriod = getBuzzerPeriod(voice, channel);
                if (square) {
                    tonePeriod = getTonePeriod(buzzerPeriod, channel);
                }
            } else if (square && !baseBuzzer) {
                tonePeriod = getTonePeriod(voice, channel);
                if (buzzer) {
                    buzzerPeriod = getBuzzerPeriod(tonePeriod, channel);
                }
            }
            if (buzzer) {
                sg->setEnvelopePeriod(buzzerPeriod);
            } else {
                sg->setLevel(index, getLevel(voice, channel));
            }
            if (square) {
                sg->setTonePeriod(index, tonePeriod);
            }
            sg->enableNoise(index, channel->noisePeriod > 0);
            if (channel->noisePeriod > 0) {
                sg->setNoisePeriod(channel->noisePeriod);
            }
            sg->setPan(index, channel->pan);
        }
    }

    void SynthEngine::process(float *left, float *right, const uint32_t size) {
        int reminder = size;
        while (reminder > 0) {
            if (updateCounter >= updatePeriod) {
                updateCounter -= updatePeriod;
                synch();
            }
            int nextUpdate = updatePeriod - updateCounter;
            if (nextUpdate >= reminder) {
                updateCounter = updateCounter + reminder;
                sg->process(left, right, reminder);
                return;
            }
            sg->process(left, right, nextUpdate);
            left += nextUpdate;
            right += nextUpdate;
            reminder -= nextUpdate;
            updateCounter += nextUpdate;
        }
    }

    int SynthEngine::spreadInt(const int value, const int bits, const int max) const {
        return value / ((1 << bits) / max);
    }

    float SynthEngine::unsignedFloat(const int value, const int bits) const {
        return (signedFloat(value, bits) + 1.0f) / 2.0f;
    }

    float SynthEngine::signedFloat(const int value, const int bits) const {
        return (value + (value == 0) - (1 << (bits - 1))) / (float)((1 << (bits - 1)) - 1);
    }

    int SynthEngine::getLevel(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const {
        return (int)(voice->envelopeLevel * voice->velocity * channel->volume / 128.0 * 16.0);
    }

    int SynthEngine::freqToTonePeriod(const double freq) const {
        return std::round(sg->clockRate / 16.0 / freq);
    }

    int SynthEngine::freqToBuzzerPeriod(const double freq) const {
        return std::round(sg->clockRate / 256.0 / freq);
    }

    int SynthEngine::getTonePeriod(const double note) const {
        double freq = 440.0 * pow(2, (note - 69) / 12.0);
        return freqToTonePeriod(freq);
    }

    int SynthEngine::getBuzzerPeriod(const double note) const {
        double freq = 440.0 * pow(2, (note - 69) / 12.0);
        return freqToBuzzerPeriod(freq);
    }

    int SynthEngine::getTonePeriod(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const {
        return getTonePeriod(voice->note + voice->envelopePitch + channel->pitchBend * 12.0f);
    }

    int SynthEngine::getTonePeriod(const int buzzerPeriod, const std::shared_ptr<Channel> channel) const {
        return buzzerPeriod * (1 << channel->multRatio) - channel->multDetune;
    }

    float SynthEngine::buzzerPeriodMult(const std::shared_ptr<Channel> channel) const {
        return channel->buzzerWaveform == 2 || channel->buzzerWaveform == 6 ? 0.5f : 1.0f;
    }

    int SynthEngine::getBuzzerPeriod(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const {
        return getBuzzerPeriod(voice->note + voice->envelopePitch + channel->pitchBend * 12.0f) * buzzerPeriodMult(channel);
    }

    int SynthEngine::getBuzzerPeriod(const int tonePeriod, const std::shared_ptr<Channel> channel) const {
        return std::round(tonePeriod / (float)(1 << channel->multRatio)) * buzzerPeriodMult(channel) - channel->multDetune;
    }

    void SynthEngine::updateEnvelope(std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) {
        if (voice->release && channel->release) {
            if (voice->releaseCounter == 0) {
                voice->releaseStartLevel = voice->envelopeLevel;
            }
            if (voice->releaseCounter >= channel->release) {
                voice->remove = true;
            } else {
                voice->envelopeLevel = voice->releaseStartLevel * (1.0f - (float)voice->releaseCounter / channel->release);
                voice->releaseCounter++;
            }
            return;
        }

        if (voice->envelopeCounter >= (channel->attack + channel->hold + channel->decay)) {
            if (channel->sustain == 0.0f) {
                voice->remove = true;
                return;
            }
            voice->envelopeLevel = channel->sustain;
            return;
        }

        auto counter = voice->envelopeCounter;
        voice->envelopeCounter++;
        counter -= channel->attack;
        if (channel->attackPitch && counter < channel->hold) {
            voice->envelopePitch = channel->attackPitch;
        }
        if (counter < 0) {
            voice->envelopeLevel = (counter + 1.0f) / channel->attack;
            return;
        }
        counter -= channel->hold;
        if (counter < 0) {
            voice->envelopeLevel = 1.0f;
            return;
        }
        voice->envelopePitch = 0;
        if (counter < channel->decay) {
            voice->envelopeLevel = 1.0f + (channel->sustain - 1.0f) * ((float)counter / channel->decay);
            return;
        }
    }

    MidiMsgStatus SynthEngine::getMidiMsgStatus(const uint8_t* msg) {
        uint8_t hNibble = msg[0] & 0xF0;

        if (hNibble == 0xF0) {
            return (MidiMsgStatus)msg[0];
        }

        return (MidiMsgStatus)hNibble;
    }

}
