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
            int lastEnvShape = 0;

        public:
            SoundGenerator(double sampleRate, int clockRate);
            std::shared_ptr<struct ayumi> getAyumi();
            int getSampleRate();
            int setClockRate(int clockRate);
            int getClockRate() const;
            void setEmul(Emul emul);
            void enableRemoveDc(bool enable = true);
            void setGain(float gain);
            int freqToSquarePeriod(const double freq) const;
            int freqToBuzzerPeriod(const double freq) const;
            void setNoisePeriod(int period);
            void setEnvelopePeriod(int period);
            void setEnvelopeFreq(int freq);
            void setEnvelopeShape(int shape);
            void process(float *left, float *right, const uint32_t size);
    };
}
