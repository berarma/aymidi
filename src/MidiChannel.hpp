#pragma once

#include <memory>
#include <vector>

namespace AyMidi {

    class Voice {
        public:
            bool isNew;
            bool remove;
            int channelId;
            int note;
            int velocity;
            int pressure;
            Voice(int channelId, int note, int velocity) :
                isNew(true),
                remove(false),
                channelId(channelId),
                note(note),
                velocity(velocity)
            { }
    };

    class MidiChannel {
        private:
            std::vector<std::shared_ptr<Voice>> allocatedVoices;
            int index;
        public:
            int program;
            float pressure;
            float pitchBend;
            float modWheel;
            float pan;
            float volume;
            int noisePeriod;
            int multRatio;
            int multDetune;
            int arpeggioSpeed;
            int attackPitch;
            int attack;
            int hold;
            int decay;
            float sustain;
            int release;
            float ringmodDepth;
            float ringmodDetune;
            float ringmodDuty;
            float syncBuzzerPeriod;

            MidiChannel(const int index);
            std::shared_ptr<Voice> cmdNoteOn(const int note, const int velocity);
            std::shared_ptr<Voice> cmdNoteOff(const int note, const int velocity);
            void cmdKeyPressure(const int note, const int pressure);
            void cmdAllNotesOff();
            void cmdReset();
            void cmdResetCC();
    };

}
