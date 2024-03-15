#include <algorithm>
#include <cmath>
#include "SynthEngine.hpp"
#include "DistrhoUtils.hpp"

namespace AyMidi {

    SynthEngine::SynthEngine(std::shared_ptr<SoundGenerator> sg) : sg(sg) {
        vp = std::make_shared<VoiceProcessor>(sg);
        vp->setOmniMode(true);
        vp->setMonoMode(false);

        for (int index = 0; index < 16; index++) {
            channels[index] = std::make_unique<Channel>(vp, index);
        }

        baseChannel = 0;
        lastChannel = 0;

        setUpdateRate(50);
    }

    void SynthEngine::setUpdateRate(int rate) {
        updateRate = rate;
        updatePeriod = std::round((float)sg->getSampleRate() / rate);
        updateCounter = 0;
    }

    void SynthEngine::setBasicChannel(int nChannel) {
        baseChannel = nChannel;
    }

    void SynthEngine::allNotesOff() {
        for (int i = 0; i < 16; i++) {
            channels[i]->msgAllNotesOff();
        }
    }

    MidiMsgStatus SynthEngine::getMidiMsgStatus(const uint8_t* msg) {
        uint8_t hNibble = msg[0] & 0xF0;
        if (hNibble == 0xF0) {
            return (MidiMsgStatus)msg[0];
        }
        return (MidiMsgStatus)hNibble;
    }

    void SynthEngine::midiSend(const uint8_t* message) {
        const uint8_t status = message[0];
        const int index = status & 0xF;

        if (!vp->getOmniMode()) {
            if (index < baseChannel || index > lastChannel) {
                return;
            }
        }

        Channel* channel = &*channels[index];

        switch (getMidiMsgStatus(message)) {
            case MIDI_MSG_NOTE_OFF:
                channel->msgNoteOff(message[1], message[2]);
                break;
            case MIDI_MSG_NOTE_ON:
                channel->msgNoteOn(message[1], message[2]);
                break;
            case MIDI_MSG_KEY_PRESSURE:
                channel->msgKeyPressure(message[1], message[2]);
                break;
            case MIDI_MSG_CHANNEL_PRESSURE:
                channel->msgPressure(message[1]);
                break;
            case MIDI_MSG_PITCH_BEND:
                channel->msgPitchBend(message[1], message[2]);
                break;
            case MIDI_MSG_PGM_CHANGE:
                channel->msgProgramChange(message[1]);
                break;
            case MIDI_MSG_RESET:
                channel->msgReset();
                break;
            case MIDI_MSG_CONTROL:
                if (index != baseChannel && message[1] >= MIDI_CTL_ALL_SOUNDS_OFF) {
                    break;
                }
                switch (message[1]) {
                    case MIDI_CTL_RESET_CONTROLLERS:
                        channel->msgResetCC();
                        break;
                    case MIDI_CTL_ALL_SOUNDS_OFF:
                        for (int i = 0; i < 16; i++) {
                            channels[i]->msgAllSoundsOff();
                        }
                        break;
                    case MIDI_CTL_ALL_NOTES_OFF:
                        allNotesOff();
                        break;
                    case MIDI_CTL_OMNI_MODE_ON:
                        vp->setOmniMode(true);
                        allNotesOff();
                        break;
                    case MIDI_CTL_OMNI_MODE_OFF:
                        vp->setOmniMode(false);
                        allNotesOff();
                        break;
                    case MIDI_CTL_POLY_MODE_ON:
                        vp->setMonoMode(false);
                        lastChannel = baseChannel;
                        allNotesOff();
                        break;
                    case MIDI_CTL_MONO_MODE_ON:
                        vp->setMonoMode(true);
                        if (message[2] == 0) {
                            lastChannel = std::min(baseChannel + 2, 15);
                        } else {
                            lastChannel = std::min(baseChannel + message[2] - 1, 15);
                        }
                        allNotesOff();
                        break;
                    case MIDI_CTL_MSB_MODWHEEL:
                        channel->msgModWheel(message[2]);
                        break;
                    case MIDI_CTL_MSB_PAN:
                        channel->msgPan(message[2]);
                        break;
                    case MIDI_CTL_MSB_MAIN_VOLUME:
                        channel->msgVolume(message[2]);
                        break;
                    /* AY/YM Effects */
                    case MIDI_CTL_AY_NOISE_PERIOD:
                        channel->msgNoisePeriod(message[2]);
                        break;
                    case MIDI_CTL_AY_BUZZER_DETUNE:
                        channel->msgBuzzerDetune(message[2]);
                        break;
                    case MIDI_CTL_AY_SQUARE_DETUNE:
                        channel->msgSquareDetune(message[2]);
                        break;
                    case MIDI_CTL_AY_ATTACK_PITCH:
                        channel->msgAttackPitch(message[2]);
                        break;
                    case MIDI_CTL_AY_ATTACK:
                        channel->msgAttack(message[2]);
                        break;
                    case MIDI_CTL_AY_HOLD:
                        channel->msgHold(message[2]);
                        break;
                    case MIDI_CTL_AY_DECAY:
                        channel->msgDecay(message[2]);
                        break;
                    case MIDI_CTL_AY_SUSTAIN:
                        channel->msgSustain(message[2]);
                        break;
                    case MIDI_CTL_AY_RELEASE:
                        channel->msgRelease(message[2]);
                        break;
                    case MIDI_CTL_AY_ARPEGGIO_RATE:
                        channel->msgArpeggioRate(message[2]);
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }

    void SynthEngine::process(float *left, float *right, const uint32_t size) {
        int reminder = size;
        while (reminder > 0) {
            if (updateCounter >= updatePeriod) {
                updateCounter -= updatePeriod;
                update();
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

    void SynthEngine::update() {
        for (int index = 0; index < 16; index++) {
            channels[index]->update(updateRate);
        }
        vp->update(updateRate);
    }
}
