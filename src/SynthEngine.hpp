#pragma once

#include <cstdint>
#include <vector>
#include "SoundGenerator.hpp"
#include "Channel.hpp"

namespace AyMidi {

    struct Program {
        bool square;
        bool buzzer;
        bool fixed; // Always the non-base tone
        bool baseBuzzer;
    };

    class SynthEngine {
        private:
            struct Program programs[10] = {
                // square buzzer fixed  bBuzzr
                {  true,  false, false, false, }, // Pgm 0
                {  false, true,  false, true,  }, // Pgm 1
                {  true,  true,  false, true,  }, // Pgm 2
                {  true,  true,  false, false, }, // Pgm 3
                {  false, false, false, false, }, // Pgm 4
                {  false, false, false, false, }, // Pgm 5
                {  false, false, false, false, }, // Pgm 6
                {  false, false, false, false, }, // Pgm 7
                {  false, false, false, false, }, // Pgm 8
                {  false, false, false, false, }, // Pgm 9
            };
            std::shared_ptr<SoundGenerator> sg;
            std::shared_ptr<Channel> channels[16];
            std::shared_ptr<Voice> voices[3];
            std::vector<int> voicePool;
            unsigned updatePeriod;
            unsigned updateCounter;

            int getLevel(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const;
            int freqToTonePeriod(const double freq) const;
            int freqToBuzzerPeriod(const double freq) const;
            int getTonePeriod(const double note) const;
            int getBuzzerPeriod(const double note) const;
            int getTonePeriod(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const;
            int getTonePeriod(const int buzzerPeriod, const std::shared_ptr<Channel> channel) const;
            int getBuzzerPeriod(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const;
            int getBuzzerPeriod(const int tonePeriod, const std::shared_ptr<Channel> channel) const;
            void updateEnvelope(std::shared_ptr<Voice> voice, std::shared_ptr<Channel> channel);
            float centerValue(int value, int bits) const;
            void synch();

        public:
            SynthEngine(std::shared_ptr<SoundGenerator> sg);
            void process(float *left, float *right, const uint32_t size);
            void midiSend(const uint8_t* message);
            void setNoisePeriod(const int index, const int period);
            void setMultRatio(const int index, const int ratio);
            void setMultDetune(const int index, const int detune);
            void setArpeggioSpeed(const int index, const int speed);
            void setAttackPitch(const int index, const int pitch);
            void setAttack(const int index, const int attack);
            void setHold(const int index, const int hold);
            void setDecay(const int index, const int decay);
            void setSustain(const int index, const float sustain);
            void setRelease(const int index, const int release);
            void setSyncSquarePeriod(const int index, const float period);
            void setSyncBuzzerPeriod(const int index, const float period);
            void setUpdateRate(int rate);
    };

}
