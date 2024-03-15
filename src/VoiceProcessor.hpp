#pragma once

#include <cstdint>
#include <memory>
#include "SoundGenerator.hpp"
#include "Note.hpp"
#include "Voice.hpp"

namespace AyMidi {

    class VoiceProcessor {

        private:
            std::shared_ptr<SoundGenerator> sg;
            std::shared_ptr<Voice> voices[3];
            std::shared_ptr<Note> notes[3];
            std::uint32_t tokens[3]; 
            std::uint32_t lastToken;
            bool omniMode;
            bool monoMode;

            int findFreeVoice();

        public:
            VoiceProcessor(std::shared_ptr<SoundGenerator> sg);
            void setOmniMode(bool enable);
            bool getOmniMode() const;
            void setMonoMode(bool enable);
            bool getMonoMode() const;
            void registerNote(std::shared_ptr<Note> note, int channelId);
            void update(int updateRate);
    };
}
