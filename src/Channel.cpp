#include <algorithm>
#include "Channel.hpp"

namespace AyMidi {

    Channel::Channel(const int index) {
        this->index = index;
        cmdReset();
    }

    std::shared_ptr<Voice> Channel::cmdNoteOn(const int note, const int velocity) {
        if (velocity == 0) {
            return cmdNoteOff(note, velocity);
        }
        auto voice = std::make_shared<Voice>(index, note, velocity);
        allocatedVoices.push_back(voice);
        return voice;
    }

    std::shared_ptr<Voice> Channel::cmdNoteOff(const int note, const int velocity) {
        auto it = std::find_if(allocatedVoices.begin(), allocatedVoices.end(), [note](std::shared_ptr<Voice> voice) { return voice->note == note; });
        if (it == allocatedVoices.end()) {
            return nullptr;
        }
        std::shared_ptr<Voice> voice = *it;
        allocatedVoices.erase(it);
        if (release) {
            voice->release = true;
        } else {
            voice->remove = true;
        }
        return voice;
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
        multRatio = 0;
        multDetune = 0;
        arpeggioSpeed = 0;
        attackPitch = 0;
        attack = 0;
        hold = 0;
        decay = 0;
        sustain = 1.0f;
        release = 0;
        ringmodDepth = 0.0f;
        ringmodDetune = 0.0f;
        ringmodDuty = 0.0f;
        syncBuzzerPeriod = 0.0f;
    }

    void Channel::cmdResetCC() {
        pitchBend = 0.0f;
        modWheel = 0.0f;
        pressure = 0.0f;
    }
}
