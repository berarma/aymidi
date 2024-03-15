#include "DistrhoUtils.hpp"
#include "VoiceProcessor.hpp"

namespace AyMidi {

    VoiceProcessor::VoiceProcessor(std::shared_ptr<SoundGenerator> sg) {
        this->sg = sg;
        for (int i = 0; i < 3; i++) {
            voices[i] = std::make_shared<Voice>(sg, i);
            notes[i] = nullptr;
            tokens[i] = 0;
        }
        lastToken = 0;
    }

    void VoiceProcessor::setOmniMode(bool enable) {
        omniMode = enable;
    }

    bool VoiceProcessor::getOmniMode() const {
        return omniMode;
    }

    void VoiceProcessor::setMonoMode(bool enable) {
        monoMode = enable;
    }

    bool VoiceProcessor::getMonoMode() const {
        return monoMode;
    }

    int VoiceProcessor::findFreeVoice() {
        int oldestVoiceId = 0;
        int releasedVoiceId = -1;

        for (int i = 0; i < 3; i++) {
            auto note = notes[i];
            if (note == nullptr || !note->isValid()) {
                return i;
            } else if (note->isReleased()) {
                releasedVoiceId = i;
            }
            if (tokens[i] < tokens[oldestVoiceId]) {
                oldestVoiceId = i;
            }
        }

        return std::max(releasedVoiceId, oldestVoiceId);
    }

    void VoiceProcessor::registerNote(std::shared_ptr<Note> note, int channelId) {
        int voiceId;
        if (monoMode) {
            if (omniMode) {
                voiceId = 0;
            } else {
                voiceId = channelId % 3;
            }
        } else {
            voiceId = findFreeVoice();
            if (notes[voiceId] != nullptr) {
                notes[voiceId]->setVoice(nullptr);
            }
        }
        note->setVoice(voices[voiceId]);
        notes[voiceId] = note;
        tokens[voiceId] = lastToken++;
    }

    void VoiceProcessor::update(int updateRate) {
        for (int i = 0; i < 3; i++) {
            std::shared_ptr<Note> note = notes[i];
            if (note != nullptr) {
                if (note->isValid()) {
                    note->update(updateRate);
                } else {
                    voices[i]->mute();
                    notes[i] = nullptr;
                }
            }
        }
    }
}
