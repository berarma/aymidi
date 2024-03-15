#pragma once

#include <memory>

namespace AyMidi {

    enum Emul {
        AY8910,
        YM2149
    };

    class SoundGenerator {

        private:
            const static Emul defaultEmul = YM2149;
            std::shared_ptr<struct ayumi> ayumi;
            Emul emul = defaultEmul;
            float gain;
            int clockRate;
            double sampleRate;
            double clockStep;
            bool removeDc = false;

        public:
            SoundGenerator(double sampleRate, int clockRate);
            int getSampleRate();
            int setClockRate(int clockRate);
            int getClockRate() const;
            void setEmul(Emul emul);
            void enableRemoveDc(bool enable = true);
            void setGain(float gain);
            void process(float *left, float *right, const uint32_t size);
            std::shared_ptr<struct ayumi> getAyumi();
    };

}
