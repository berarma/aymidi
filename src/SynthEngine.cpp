#include <algorithm>
#include <cmath>
#include <lv2/midi/midi.h>
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

    void SynthEngine::setNoisePeriod(const int index, const int period) {
        channels[index]->noisePeriod = period;
    }

    void SynthEngine::setMultRatio(const int index, const int ratio) {
        channels[index]->multRatio = ratio;
    }

    void SynthEngine::setMultDetune(const int index, const int detune) {
        channels[index]->multDetune = detune;
    }

    void SynthEngine::setArpeggioSpeed(const int index, const int speed) {
        channels[index]->arpeggioSpeed = speed;
    }

    void SynthEngine::setAttackPitch(const int index, const int pitch) {
        channels[index]->attackPitch = pitch;
    }

    void SynthEngine::setAttack(const int index, const int attack) {
        channels[index]->attack = attack;
    }

    void SynthEngine::setHold(const int index, const int hold) {
        channels[index]->hold = hold;
    }

    void SynthEngine::setDecay(const int index, const int decay) {
        channels[index]->decay = decay;
    }

    void SynthEngine::setSustain(const int index, const float sustain) {
        channels[index]->sustain = sustain;
    }

    void SynthEngine::setRelease(const int index, const int release) {
        channels[index]->release = release;
    }

    void SynthEngine::setRingmodDepth(const int index, const float depth) {
        channels[index]->ringmodDepth = depth;
    }

    void SynthEngine::setRingmodDetune(const int index, const float detune) {
        channels[index]->ringmodDetune = detune;
    }

    void SynthEngine::setRingmodDuty(const int index, const float duty) {
        channels[index]->ringmodDuty = duty;
    }

    void SynthEngine::setSyncBuzzerPeriod(const int index, const float period) {
        channels[index]->syncBuzzerPeriod = period;
    }

    void SynthEngine::midiSend(const uint8_t* message) {
        const uint8_t status = message[0];
        const int index = status & 0xF;

        Channel* channel = &*channels[index];

        switch (lv2_midi_message_type(message)) {
            case LV2_MIDI_MSG_NOTE_OFF:
                channel->cmdNoteOff(message[1], message[2]);
                break;
            case LV2_MIDI_MSG_NOTE_ON:
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
            case LV2_MIDI_MSG_NOTE_PRESSURE:
                channel->cmdKeyPressure(message[1], message[2]);
                break;
            case LV2_MIDI_MSG_CHANNEL_PRESSURE:
                channel->pressure = message[1] / 127.0f;
                break;
            case LV2_MIDI_MSG_BENDER:
                channel->pitchBend = centerValue(message[1] + (message[2] << 7), 14);
                break;
            case LV2_MIDI_MSG_PGM_CHANGE:
                channel->program = std::min<uint8_t>(39, message[1]);
                break;
            case LV2_MIDI_MSG_RESET:
                channel->cmdReset();
                break;
            case LV2_MIDI_MSG_CONTROLLER:
                switch (message[1]) {
                    case LV2_MIDI_CTL_RESET_CONTROLLERS:
                        channel->cmdResetCC();
                        break;
                    case LV2_MIDI_CTL_ALL_SOUNDS_OFF:
                    case LV2_MIDI_CTL_ALL_NOTES_OFF:
                        channel->cmdAllNotesOff();
                        break;
                    case LV2_MIDI_CTL_MSB_MODWHEEL:
                        channel->modWheel = centerValue(message[2], 7);
                        break;
                    case LV2_MIDI_CTL_MSB_PAN:
                        channel->pan = (centerValue(message[2], 7) + 1.0f) / 2.0f;
                        break;
                    case LV2_MIDI_CTL_MSB_MAIN_VOLUME:
                        channel->volume = message[2] / 127.0;
                        break;
                }
                break;
            default:
                break;
        }
    }

    void SynthEngine::synch() {
        for (int index = 0; index < 3; index++) {
            const auto& voice = voices[index];
            if (voice == nullptr) {
                continue;
            }
            if (voice->remove) {
                sg->setLevel(index, 0);
                sg->enableEnvelope(index, false);
                auto poolIndex = std::find(voicePool.begin(), voicePool.end(), index);
                std::rotate(poolIndex, poolIndex + 1, voicePool.end());
                voices[index] = nullptr;
                continue;
            }
            const auto& channel = channels[voice->channelId];
            auto pgm = channel->program;
            int waveform = pgm / 10;
            pgm = pgm % 10;
            if (voice->isNew) {
                voice->isNew = false;
                sg->enableEnvelope(index, programs[pgm].buzzer);
                sg->enableTone(index, programs[pgm].square);
                if (programs[pgm].buzzer) {
                    sg->setEnvelopeShape(waveform * 2 + 8);
                } else {
                    sg->setLevel(index, getLevel(voice, channel));
                }
                sg->enableNoise(index, channel->noisePeriod > 0);
                d_debug("AY Channel: %d", index);
                d_debug("note: %d", voice->note);
                d_debug("velocity: %d", voice->velocity);
            }
            int tonePeriod;
            int buzzerPeriod;
            if (programs[pgm].buzzer && programs[pgm].baseBuzzer) {
                buzzerPeriod = getBuzzerPeriod(voice, channel);
                if (programs[pgm].square && !programs[pgm].fixed) {
                    tonePeriod = getTonePeriod(buzzerPeriod, channel);
                }
            } else if (programs[pgm].square && !programs[pgm].baseBuzzer) {
                tonePeriod = getTonePeriod(voice, channel);
                if (programs[pgm].buzzer && !programs[pgm].fixed) {
                    buzzerPeriod = getBuzzerPeriod(tonePeriod, channel);
                }
            }
            if (programs[pgm].buzzer) {
                d_debug("buzzer on: %d", buzzerPeriod);
                sg->setEnvelopePeriod(buzzerPeriod);
            }
            if (programs[pgm].square) {
                d_debug("tone on: %d", tonePeriod);
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

    float SynthEngine::centerValue(int value, int bits) const {
        return (value + (value == 0) - (1 << (bits - 1))) / (float)((1 << (bits - 1)) - 1);
    }

    int SynthEngine::getLevel(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const {
        return std::round(voice->velocity * channel->volume / 127.0 * 15.0);
    }

    int SynthEngine::freqToTonePeriod(const double freq) const {
        return std::round(sg->getClockRate() / 16.0 / freq);
    }

    int SynthEngine::freqToBuzzerPeriod(const double freq) const {
        return std::round(sg->getClockRate() / 256.0 / freq);
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
        return getTonePeriod(voice->note + channel->pitchBend * 12.0);
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

}
