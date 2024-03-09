#pragma once

#include <memory>
#include <vector>

namespace AyMidi {

    class Voice {
        public:
            bool isNew;
            bool remove;
            bool release;
            int channelId;
            int note;
            int velocity;
            int pressure;
            float envelopeLevel;
            float releaseStartLevel;
            unsigned envelopeCounter;
            unsigned releaseCounter;
            float envelopePitch;
            Voice(int channelId, int note, int velocity) :
                isNew(true),
                remove(false),
                release(false),
                channelId(channelId),
                note(note),
                velocity(velocity),
                envelopeCounter(0),
                releaseCounter(0),
                envelopeLevel(0.0f),
                releaseStartLevel(0.0f),
                envelopePitch(0.0f)
            { }
    };

    class Channel {
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
            int buzzerWaveform;
            bool mixBoth;
            int noisePeriod;
            int multRatio;
            int multDetune;
            int arpeggioSpeed;
            float attackPitch;
            int attack;
            int hold;
            int decay;
            float sustain;
            int release;
            float ringmodDepth;
            float ringmodDetune;
            float ringmodDuty;
            float syncSquarePeriod;
            float syncBuzzerPeriod;

            Channel(const int index);
            std::shared_ptr<Voice> cmdNoteOn(const int note, const int velocity);
            std::shared_ptr<Voice> cmdNoteOff(const int note, const int velocity);
            void cmdKeyPressure(const int note, const int pressure);
            void cmdAllNotesOff();
            void cmdReset();
            void cmdResetCC();
    };

}
