#pragma once

#include <memory>

namespace AyMidi {

    enum Emul {
        AY8910,
        YM2149
    };

    struct AyChannel {
        bool tone_off = true;
        bool noise_off = true;
        bool envelope_on = false;
    };

    class SoundGenerator {
        private:
            std::shared_ptr<struct ayumi> ayumi;
            int clockRate;
            Emul emul = defaultEmul;
            bool removeDc = false;
            float gain;
            AyChannel channels[3];
        public:
            double sampleRate;

            const static Emul defaultEmul = YM2149;
            SoundGenerator(double sampleRate, int clockRate);
            int setClockRate(int clockRate);
            void setEmul(Emul emul);
            void enableRemoveDc(bool enable = true);
            void setGain(float gain);
            void setNoisePeriod(int period);
            void setEnvelopePeriod(int period);
            void setEnvelopeShape(int shape);
            void enableTone(int index, bool enable = true);
            void enableNoise(int index, bool enable = true);
            void enableEnvelope(int index, bool enable = true);
            void setLevel(int index, int level);
            void setTonePeriod(int index, int period);
            void setPan(int index, float pan);
            void process(float *left, float *right, const uint32_t size);
            int getClockRate() const;
    };

}
