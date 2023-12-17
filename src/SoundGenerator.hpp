#pragma once

#include <memory>
#include "Channel.hpp"

namespace AyMidi {

    enum Emul {
        AY8910,
        YM2149
    };

    class SoundGenerator {
        private:
            std::shared_ptr<struct ayumi> ayumi;
            double sampleRate;
            int clockRate;
            Emul emul = defaultEmul;
            bool removeDc = false;
            float gain;
            float clockStep;
            Channel channels[3];
        public:
            const static Emul defaultEmul = YM2149;
            SoundGenerator(double sampleRate, int clockRate);
            Channel& getChannel(int index);
            int setClockRate(int clockRate);
            void setEmul(Emul emul);
            void enableRemoveDc(bool enable = true);
            void setGain(float gain);
            void setNoisePeriod(int period);
            void setEnvelopePeriod(int period);
            void setEnvelopeShape(int shape);
            void process(float *left, float *right, const uint32_t size);
            int getClockRate() const;
    };

}
