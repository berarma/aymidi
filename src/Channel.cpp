#include <algorithm>
#include "Channel.hpp"

namespace AyMidi {

    Channel::Channel(const int index) {
        this->index = index;
        cmdReset();
    }

    std::shared_ptr<Voice> Channel::cmdNoteOn(const int note, const int velocity) {
        if (velocity == 0) {
            cmdNoteOff(note, velocity);
            return nullptr;
        }
        allocatedVoices.erase(std::remove_if(allocatedVoices.begin(), allocatedVoices.end(), [note](std::shared_ptr<Voice> voice) {
            if (voice->note == note) {
                voice->remove = true;
                return true;
            }
            return false;
        }), allocatedVoices.end());
        auto voice = std::make_shared<Voice>(index, note, velocity);
        allocatedVoices.push_back(voice);
        return voice;
    }

    void Channel::cmdNoteOff(const int note, const int velocity) {
        std::for_each(allocatedVoices.begin(), allocatedVoices.end(), [note,release=release](std::shared_ptr<Voice> voice) {
            if (voice->note == note) {
                if (release) {
                    voice->release = true;
                } else {
                    voice->remove = true;
                }
            }
        });
    }

    void Channel::purge() {
        allocatedVoices.erase(std::remove_if(allocatedVoices.begin(), allocatedVoices.end(), [](std::shared_ptr<Voice> voice) { return voice->remove; }), allocatedVoices.end());
    }

    std::vector<std::shared_ptr<Voice>> Channel::getVoices() {
        std::sort(allocatedVoices.begin(), allocatedVoices.end(), [arpeggioSpeed = arpeggioSpeed](std::shared_ptr<Voice> a, std::shared_ptr<Voice> b) { return arpeggioSpeed > 0 ? a->note < b->note : a->note > b->note; });

        return allocatedVoices;
    }

    void Channel::cmdKeyPressure(const int note, const int pressure) {
        auto it = std::find_if(allocatedVoices.begin(), allocatedVoices.end(), [note](std::shared_ptr<Voice> voice) { return voice->note == note; });
        (*it)->pressure = pressure;
    }

    void Channel::cmdAllNotesOff() {
        for (auto& voice : allocatedVoices) {
            voice->remove = true;
        }
    }

    void Channel::cmdReset() {
        cmdAllNotesOff();
        cmdResetCC();
        program = 0;
        volume = 100.0f / 127.0f;
        pan = 0.5f;
        noisePeriod = 0;
        multRatio = 4;
        multDetune = 0;
        arpeggioSpeed = 0;
        attackPitch = 0;
        attack = 0;
        hold = 0;
        decay = 0;
        sustain = 1.0f;
        release = 0;
        buzzerWaveform = 0;
        mixBoth = false;
    }

    void Channel::cmdResetCC() {
        pitchBend = 0.0f;
        modWheel = 0.0f;
        pressure = 0.0f;
    }
}
