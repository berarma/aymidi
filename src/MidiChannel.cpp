#include <algorithm>
#include "MidiChannel.hpp"

namespace AyMidi {

    MidiChannel::MidiChannel(const int index) {
        this->index = index;
        cmdReset();
    }

    std::shared_ptr<Voice> MidiChannel::cmdNoteOn(const int note, const int velocity) {
        if (velocity == 0) {
            return cmdNoteOff(note, velocity);
        }
        auto voice = std::make_shared<Voice>(index, note, velocity);
        allocatedVoices.push_back(voice);
        return voice;
    }

    std::shared_ptr<Voice> MidiChannel::cmdNoteOff(const int note, const int velocity) {
        auto it = std::find_if(allocatedVoices.begin(), allocatedVoices.end(), [note](std::shared_ptr<Voice> voice) { return voice->note == note; });
        if (it == allocatedVoices.end()) {
            return nullptr;
        }
        std::shared_ptr<Voice> voice = *it;
        allocatedVoices.erase(it);
        voice->remove = true;
        return voice;
    }

    void MidiChannel::cmdKeyPressure(const int note, const int pressure) {
        auto it = std::find_if(allocatedVoices.begin(), allocatedVoices.end(), [note](std::shared_ptr<Voice> voice) { return voice->note == note; });
        (*it)->pressure = pressure;
    }

    void MidiChannel::cmdAllNotesOff() {
        for (auto& voice : allocatedVoices) {
            voice->remove = true;
        }
    }

    void MidiChannel::cmdReset() {
        cmdAllNotesOff();
        cmdResetCC();
        program = 0;
        volume = 100.0f / 127.0f;
        pan = 0.5f;
    }

    void MidiChannel::cmdResetCC() {
        pitchBend = 0.0f;
        modWheel = 0.0f;
        pressure = 0.0f;
    }
}
