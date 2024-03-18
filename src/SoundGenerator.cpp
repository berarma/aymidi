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

    std::shared_ptr<struct ayumi> SoundGenerator::getAyumi() {
        return ayumi;
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

    int SoundGenerator::freqToSquarePeriod(const double freq) const {
        return std::min((int)std::round(clockRate / 16.0f / freq), 0x0FFF);
    }

    int SoundGenerator::freqToBuzzerPeriod(const double freq) const {
        return std::min((int)std::round(clockRate / 256.0f / freq), 0xFFFF);
    }

    void SoundGenerator::setNoisePeriod(int period) {
        ayumi_set_noise(&*ayumi, period);
    }

    void SoundGenerator::setEnvelopePeriod(int period) {
        ayumi_set_envelope(&*ayumi, period);
    }

    void SoundGenerator::setEnvelopeFreq(int freq) {
        ayumi_set_envelope(&*ayumi, freqToBuzzerPeriod(freq));
    }

    void SoundGenerator::setEnvelopeShape(int shape) {
        if (shape != lastEnvShape) {
            ayumi_set_envelope_shape(&*ayumi, shape);
            lastEnvShape = shape;
        }
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
}
