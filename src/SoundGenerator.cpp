#include "DistrhoUtils.hpp"
#include "SoundGenerator.hpp"

extern "C" double YM_dac_table[];
extern "C" double AY_dac_table[];

namespace AyMidi {

    extern "C" {
#include "ayumi.h"
    }

    SoundGenerator::SoundGenerator(double sampleRate, int clockRate) :
        sampleRate(sampleRate),
        ayumi(std::make_shared<struct ayumi>())
    {
        ayumi_configure(&*ayumi, emul, clockRate, sampleRate);
        setClockRate(clockRate);
    }

    int SoundGenerator::getSampleRate() {
        return sampleRate;
    }

    int SoundGenerator::setClockRate(int clockRate) {
        ayumi->step = clockRate / (sampleRate * 8 * DECIMATE_FACTOR); // XXX Ayumi internals
        this->clockRate = clockRate;
        this->clockStep = clockRate / sampleRate;
        return ayumi->step < 1;
    }

    int SoundGenerator::getClockRate() const {
        return clockRate;
    }

    void SoundGenerator::setEmul(Emul emul) {
        ayumi->dac_table = emul == YM2149 ? YM_dac_table : AY_dac_table; // XXX Ayumi internals
    }

    void SoundGenerator::enableRemoveDc(bool enable) {
        removeDc = enable;
    }

    void SoundGenerator::setGain(float gain) {
        this->gain = gain;
    }

    void SoundGenerator::process(float* left, float* right, const uint32_t size) {
        for (int i = 0; i < size; i++) {
            ayumi_process(&*ayumi, false);
            if (removeDc) {
                ayumi_remove_dc(&*ayumi);
            }
            *left = (float) ayumi->left * gain;
            *right = (float) ayumi->right * gain;
            left++;
            right++;
        }
    }

    std::shared_ptr<struct ayumi> SoundGenerator::getAyumi() {
        return ayumi;
    }
}
