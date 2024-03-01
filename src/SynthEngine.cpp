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
                channel->pitchBend = centerValue(message[1] + (message[2] << 7), 14);
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
                        channel->modWheel = centerValue(message[2], 7);
                        break;
                    case MIDI_CTL_MSB_PAN:
                        channel->pan = (centerValue(message[2], 7) + 1.0f) / 2.0f;
                        break;
                    case MIDI_CTL_MSB_MAIN_VOLUME:
                        channel->volume = message[2] / 127.0;
                        break;
                    /* AY/YM Effects */
                    case MIDI_CTL_AY_NOISE_PERIOD:
                        channels[index]->noisePeriod = message[2] / 4;
                        break;
                    case MIDI_CTL_AY_BUZ_SQR_RATIO:
                        channels[index]->multRatio = message[2] / 16;
                        break;
                    case MIDI_CTL_AY_BUZ_SQR_DETUNE:
                        channels[index]->multDetune = message[2] - 64;
                        break;
                    case MIDI_CTL_AY_ARPEGGIO_SPEED:
                        channels[index]->arpeggioSpeed = message[2] - 64;
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
                    case MIDI_CTL_AY_SYNCSQUARE_PERIOD:
                        channels[index]->syncSquarePeriod = message[2] / 127.0f / 2.0f + 0.5f;
                        break;
                    case MIDI_CTL_AY_SYNCBUZZER_PERIOD:
                        channels[index]->syncBuzzerPeriod = message[2] / 127.0f;
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
            const int waveform = (channel->program - 1) / 3;
            const bool buzzer = channel->program > 0;
            const bool square = ((channel->program - 1) % 3) != 0;
            const bool baseBuzzer = channel->program != 0 && ((channel->program - 1) % 3) < 2;
            if (voice->isNew) {
                voice->isNew = false;
                sg->enableEnvelope(index, buzzer);
                sg->enableTone(index, square);
                if (buzzer) {
                    sg->setEnvelopeShape(waveform * 2 + 8);
                } else {
                    sg->setLevel(index, getLevel(voice, channel));
                }
                sg->enableNoise(index, channel->noisePeriod > 0);
            }
            if (!buzzer) {
                updateEnvelope(voice, channel);
                sg->setLevel(index, getLevel(voice, channel));
            }
            if (voice->remove) {
                sg->setLevel(index, 0);
                sg->enableEnvelope(index, false);
                auto poolIndex = std::find(voicePool.begin(), voicePool.end(), index);
                std::rotate(poolIndex, poolIndex + 1, voicePool.end());
                voices[index] = nullptr;
                continue;
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
                if (channel->syncBuzzerPeriod == 1.0f) {
                    sg->setSyncBuzzer(0);
                } else {
                    sg->setSyncBuzzer(buzzerPeriod);
                    buzzerPeriod = std::round(buzzerPeriod / channel->syncBuzzerPeriod);
                }
                sg->setEnvelopePeriod(buzzerPeriod);
            }
            if (square) {
                if (channel->syncSquarePeriod == 1.0f) {
                    sg->setSyncSquare(index, 0);
                } else {
                    sg->setSyncSquare(index, tonePeriod);
                    tonePeriod = std::round(tonePeriod / channel->syncSquarePeriod);
                }
                sg->setTonePeriod(index, tonePeriod);
            }
            sg->enableNoise(index, channel->noisePeriod > 0);
            if (channel->noisePeriod > 0) {
                sg->setNoisePeriod(channel->noisePeriod);
            }
            sg->setPan(index, channel->pan);
            if (voice->envelopeCounter < (channel->attack + channel->hold + channel->decay)) {
                voice->envelopeCounter++;
            }
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

    float SynthEngine::centerValue(int value, int bits) const {
        return (value + (value == 0) - (1 << (bits - 1))) / (float)((1 << (bits - 1)) - 1);
    }

    int SynthEngine::getLevel(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const {
        return std::round(voice->envelopeLevel * voice->velocity * channel->volume / 127.0 * 15.0);
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
        int note = voice->note;
        if (channel->attackPitch && voice->envelopeCounter < (channel->attack + channel->hold)) {
            note += channel->attackPitch;
        }
        return getTonePeriod(note + channel->pitchBend * 12.0);
    }

    int SynthEngine::getTonePeriod(const int buzzerPeriod, const std::shared_ptr<Channel> channel) const {
        return buzzerPeriod * (1 << channel->multRatio) - channel->multDetune;
    }

    int SynthEngine::getBuzzerPeriod(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const {
        return getBuzzerPeriod(voice->note + channel->pitchBend * 12.0);
    }

    int SynthEngine::getBuzzerPeriod(const int tonePeriod, const std::shared_ptr<Channel> channel) const {
        return std::round(tonePeriod / (float)(1 << channel->multRatio)) - channel->multDetune;
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

        auto counter = voice->envelopeCounter;
        if (counter < channel->attack) {
            voice->envelopeLevel = (counter + 1.0f) / channel->attack;
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
        if (channel->sustain == 0.0f) {
            voice->remove = true;
            return;
        }
        voice->envelopeCounter = std::numeric_limits<unsigned>::max();
        voice->envelopeLevel = channel->sustain;
    }

    MidiMsgStatus SynthEngine::getMidiMsgStatus(const uint8_t* msg) {
        uint8_t hNibble = msg[0] & 0xF0;

        if (hNibble == 0xF0) {
            return (MidiMsgStatus)msg[0];
        }

        return (MidiMsgStatus)hNibble;
    }

}
