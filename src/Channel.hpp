#pragma once

#include <memory>

extern "C"
{
#include "ayumi.h"
}

namespace AyMidi {

    class Channel {
        private:
            std::shared_ptr<struct ayumi> ayumi;
            int index;
            int tone_off;
            int noise_off;
            int envelope_on;
            float detune;
            float modulationPeriod;
            int modulationAmplitude;
        public:
            Channel(std::shared_ptr<struct ayumi> ayumi, int index);
            void enableTone(bool enable = true);
            void enableNoise(bool enable = true);
            void enableEnvelope(bool enable = true);
            void setLevel(int level);
            void setTonePeriod(int period);
            void setPan(float pan);
    };

}
