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
        voice->remove = true;
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
    }

    void Channel::cmdResetCC() {
        pitchBend = 0.0f;
        modWheel = 0.0f;
        pressure = 0.0f;
    }
}
