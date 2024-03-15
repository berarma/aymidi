#pragma once

#include <memory> 
#include "SoundGenerator.hpp"

namespace AyMidi {

    class Voice {

        private:
            int index;
            bool toneOff = true;
            bool noiseOff = true;
            bool envelopeOn = false;
            int tonePeriod;
            int syncSquarePeriod;
            double syncSquareCounter;

        public:
            std::shared_ptr<SoundGenerator> sg;

            Voice(std::shared_ptr<SoundGenerator> sg, int index);
            void mute();
            void setNoisePeriod(int period);
            void setEnvelopePeriod(int period);
            void setEnvelopeShape(int shape);
            void enableTone(bool enable = true);
            void enableNoise(bool enable = true);
            void enableEnvelope(bool enable = true);
            void setLevel(int level);
            void setTonePeriod(int period);
            void setPan(float pan);
            void setSyncSquare(int period);
            void setSyncBuzzer(int period);
            int freqToSquarePeriod(const double freq) const;
            int freqToBuzzerPeriod(const double freq) const;
    };
}
