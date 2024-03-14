#include <algorithm>
#include <cmath>
#include "SynthEngine.hpp"
#include "DistrhoUtils.hpp"

namespace AyMidi {

    SynthEngine::SynthEngine(std::shared_ptr<SoundGenerator> sg) : sg(sg) {
        for (int i = 0; i < 16; i++) {
            channels[i] = std::make_unique<Channel>(i);
        }
        for (int i = 0; i < 3; i++) {
            voices[i] = nullptr;
            voicePool.push_back(i);
        }

        basicChannel = 0;
        lastChannel = 0;
        omniMode = true;
        polyMode = true;

        setUpdateRate(50);
    }

    void SynthEngine::setUpdateRate(int rate) {
        updateRate = rate;
        updatePeriod = std::round((float)sg->sampleRate / rate);
        updateCounter = 0;
    }

    void SynthEngine::setBasicChannel(int nChannel) {
        basicChannel = nChannel;
    }

    void SynthEngine::allNotesOff() {
        for (int i = 0; i < 16; i++) {
            channels[i]->cmdAllNotesOff();
        }
    }

    void SynthEngine::noteOn(Channel* channel, int note, int velocity) {
        auto voice = channel->cmdNoteOn(note, velocity);
        if (voice == nullptr) {
            return;
        }
        if (polyMode) {
            auto slotIndex = voicePool.back();
            if (voices[slotIndex] != nullptr) {
                channel->cmdNoteOff(voices[slotIndex]->note, 0);
            }
            voices[slotIndex] = voice;
            std::rotate(voicePool.begin(), voicePool.end() - 1, voicePool.end());
        } else {
            auto slotIndex = channel->index & 3;
            voices[slotIndex] = voice;
        }
    }

    void SynthEngine::midiSend(const uint8_t* message) {
        const uint8_t status = message[0];
        const int index = status & 0xF;

        if (!omniMode) {
            if (index < basicChannel || index > lastChannel) {
                return;
            }
        }

        Channel* channel = &*channels[index];

        switch (getMidiMsgStatus(message)) {
            case MIDI_MSG_NOTE_OFF:
                channel->cmdNoteOff(message[1], message[2]);
                break;
            case MIDI_MSG_NOTE_ON:
                noteOn(channel, message[1], message[2]);
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
                if (message[1] < 5) {
                    channel->program = message[1];
                    channel->buzzerWaveform = 4 + 2 * (channel->program / 3);
                }
                break;
            case MIDI_MSG_RESET:
                channel->cmdReset();
                break;
            case MIDI_MSG_CONTROL:
                if (index != basicChannel && message[1] >= MIDI_CTL_ALL_SOUNDS_OFF) {
                    return;
                }
                switch (message[1]) {
                    case MIDI_CTL_RESET_CONTROLLERS:
                        channel->cmdResetCC();
                        break;
                    case MIDI_CTL_ALL_SOUNDS_OFF:
                        for (int i = 0; i < 16; i++) {
                            channels[i]->cmdAllSoundsOff();
                        }
                        break;
                    case MIDI_CTL_ALL_NOTES_OFF:
                        allNotesOff();
                        break;
                    case MIDI_CTL_OMNI_MODE_ON:
                        omniMode = true;
                        allNotesOff();
                        break;
                    case MIDI_CTL_OMNI_MODE_OFF:
                        omniMode = false;
                        allNotesOff();
                        break;
                    case MIDI_CTL_POLY_MODE_ON:
                        polyMode = true;
                        lastChannel = basicChannel;
                        allNotesOff();
                        break;
                    case MIDI_CTL_MONO_MODE_ON:
                        polyMode = false;
                        if (message[2] == 0) {
                            lastChannel = std::min(basicChannel + 2, 15);
                        } else {
                            lastChannel = std::min(basicChannel + message[2] - 1, 15);
                        }
                        allNotesOff();
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
                    case MIDI_CTL_AY_BUZZER_DETUNE:
                        channels[index]->buzzerDetune = message[2] / 20.0f;
                        break;
                    case MIDI_CTL_AY_SQUARE_DETUNE:
                        channels[index]->squareDetune = message[2] / 4.0f;
                        break;
                    case MIDI_CTL_AY_ATTACK_PITCH:
                        channels[index]->attackPitch = message[2] - 64;
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
                        {
                            int period = (64 - spreadInt(message[2], 7, 64)) - (message[2] < 64 ? 65 : 0);
                            if (period == 32) {
                                period = 0;
                            }
                            channels[index]->arpeggioPeriod = std::round((float)period * updateRate / 100.0f);
                        }
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }

    void SynthEngine::synch() {
        for (int index = 0; index < 3; index++) {
            auto voice = voices[index];
            if (voice == nullptr) {
                continue;
            }
            const auto channel = channels[voice->channelId];
            if (!polyMode && channel->arpeggioPeriod != 0) {
                auto cVoices = channel->getVoices();
                std::shared_ptr<Voice> firstVoice = nullptr;
                std::shared_ptr<Voice> nextVoice = nullptr;
                for (auto aVoice : cVoices) {
                    if (!aVoice->remove) {
                        updateEnvelope(aVoice, channel);
                    }
                    if (aVoice->remove) {
                        continue;
                    }
                    if (aVoice->note != voice->note) {
                        if (firstVoice == nullptr) {
                            firstVoice = aVoice;
                            nextVoice = aVoice;
                        } else if (nextVoice == nullptr) {
                            nextVoice = aVoice;
                        }
                    } else {
                        nextVoice = nullptr;
                    }
                }
                voice->arpeggioCounter++;
                if (voice->remove || voice->arpeggioCounter >= abs(channel->arpeggioPeriod)) {
                    voice->arpeggioCounter = 0;
                    if (nextVoice != nullptr) {
                        voice = nextVoice;
                    } else if (firstVoice != nullptr) {
                        voice = firstVoice;
                    }
                }
                voices[index] = voice;
            } else {
                updateEnvelope(voice, channel);
            }

            channel->purge();

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

            const bool buzzer = channel->program > 0;
            const bool square = channel->program % 2 == 0;
            sg->enableEnvelope(index, buzzer);
            sg->enableTone(index, square);
            if (buzzer && voice->buzzerWaveform != channel->buzzerWaveform) {
                sg->setEnvelopeShape(channel->buzzerWaveform + 8);
                voice->buzzerWaveform = channel->buzzerWaveform;
                sg->syncTone(index);
            }
            int tonePeriod;
            int buzzerPeriod;
            if (buzzer) {
                buzzerPeriod = getBuzzerPeriod(voice, channel);
                sg->setEnvelopePeriod(buzzerPeriod);
            }
            if (square) {
                tonePeriod = getSquarePeriod(voice, channel);
                sg->setLevel(index, getLevel(voice, channel));
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

    int SynthEngine::freqToSquarePeriod(const double freq) const {
        return std::min((int)std::round(sg->clockRate / 16.0 / freq), 0x0FFF);
    }

    int SynthEngine::freqToBuzzerPeriod(const double freq) const {
        return std::min((int)std::round(sg->clockRate / 256.0 / freq), 0xFFFF);
    }

    float SynthEngine::getNoteFreq(const double note) const {
        return 440.0f * pow(2, (note - 69) / 12.0f);
    }

    int SynthEngine::getSquarePeriod(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const {
        float freq = getNoteFreq(voice->note + voice->envelopePitch + channel->squareDetune + channel->pitchBend * 12.0f);
        return freqToSquarePeriod(freq);
    }

    int SynthEngine::getBuzzerPeriod(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const {
        float mult = channel->buzzerWaveform == 2 || channel->buzzerWaveform == 6 ? 0.5f : 1.0f;
        float freq = getNoteFreq(voice->note + voice->envelopePitch + channel->buzzerDetune + channel->pitchBend * 12.0f);
        return freqToBuzzerPeriod(freq) * mult;
    }

    void SynthEngine::updateEnvelope(std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) {
        voice->envelopePitch = 0.0f;
        if (voice->release) {
            if (voice->releaseCounter >= channel->release) {
                voice->remove = true;
            } else {
                if (voice->releaseCounter == 0) {
                    voice->releaseStartLevel = voice->envelopeLevel;
                }
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
        if (channel->attackPitch && counter < (channel->attack + channel->hold)) {
            voice->envelopePitch = channel->attackPitch * (1.0f - ((float)counter / (channel->attack + channel->hold)));
        }
        if (counter < channel->attack) {
            voice->envelopeLevel = (float)counter / channel->attack;
            return;
        }
        counter -= channel->attack;
        if (counter < channel->hold) {
            voice->envelopeLevel = 1.0f;
            return;
        }
        counter -= channel->hold;
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
