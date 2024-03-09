#pragma once

#include <memory>

namespace AyMidi {

    enum Emul {
        AY8910,
        YM2149
    };

    struct AyChannel {
        bool toneOff = true;
        bool noiseOff = true;
        bool envelopeOn = false;
        int tonePeriod;
        int syncSquarePeriod;
        double syncSquareCounter;
        bool syncTone = false;
    };

    class SoundGenerator {
        private:
            std::shared_ptr<struct ayumi> ayumi;
            Emul emul = defaultEmul;
            float gain;
            double clockStep;
            bool removeDc = false;
            int envelopePeriod;
            int syncBuzzerPeriod;
            double syncBuzzerCounter;
            AyChannel channels[3];
        public:
            int clockRate;
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
            void syncTone(int index);
            void enableTone(int index, bool enable = true);
            void enableNoise(int index, bool enable = true);
            void enableEnvelope(int index, bool enable = true);
            void setLevel(int index, int level);
            void setTonePeriod(int index, int period);
            void setPan(int index, float pan);
            void setSyncSquare(int index, int period);
            void setSyncBuzzer(int period);
            void process(float *left, float *right, const uint32_t size);
            int getClockRate() const;
    };

}
