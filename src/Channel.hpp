#pragma once

#include <memory>
#include <vector>

namespace AyMidi {

    class Voice {
        public:
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
            int buzzerWaveform;
            int arpeggioCounter;
            Voice(int channelId, int note, int velocity) :
                remove(false),
                release(false),
                channelId(channelId),
                note(note),
                velocity(velocity),
                envelopeCounter(0),
                releaseCounter(0),
                envelopeLevel(0.0f),
                releaseStartLevel(0.0f),
                envelopePitch(0.0f),
                buzzerWaveform(-1),
                arpeggioCounter(0)
            { }
    };

    class Channel {
        private:
            std::vector<std::shared_ptr<Voice>> allocatedVoices;
        public:
            int index;
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

            Channel(const int index);
            std::shared_ptr<Voice> cmdNoteOn(const int note, const int velocity);
            std::shared_ptr<Voice> cmdNoteOff(const int note, const int velocity);
            std::vector<std::shared_ptr<Voice>> getVoices();
            void cmdKeyPressure(const int note, const int pressure);
            void cmdAllNotesOff();
            void cmdReset();
            void cmdResetCC();
    };

}
