#include "SoundGenerator.hpp"
#include "DistrhoUtils.hpp"

extern "C"
{
#include "ayumi.h"
}

extern double* YM_dac_table;
extern double* AY_dac_table;

namespace AyMidi {

    SoundGenerator::SoundGenerator(double sampleRate, int clockRate) :
        sampleRate(sampleRate),
        clockRate(clockRate),
        ayumi(std::make_shared<struct ayumi>()),
        channels{ Channel(ayumi, 0), Channel(ayumi, 1), Channel(ayumi, 2) }
    {
        ayumi_configure(&*ayumi, emul, clockRate, sampleRate);
        
        for (int index = 0; index < 3; index++) {
            ayumi_set_pan(&*ayumi, index, 0.5, 1);
        }
    }

    Channel& SoundGenerator::getChannel(int index) {
        return channels[index];
    }

    int SoundGenerator::setClockRate(int clockRate) {
        ayumi->step = clockRate / (sampleRate * 8 * DECIMATE_FACTOR); // XXX Ayumi internals
        this->clockRate = clockRate;
        return ayumi->step < 1;
    }

    void SoundGenerator::setEmul(Emul emul) {
        ayumi->dac_table = emul == YM2149 ? YM_dac_table : AY_dac_table; // XXX Ayumi internals
    }

    void SoundGenerator::enableRemoveDc(bool enable) {
        removeDc = enable;
    }

    void SoundGenerator::setNoisePeriod(int period) {
        ayumi_set_noise(&*ayumi, period);
    }

    void SoundGenerator::setEnvelopePeriod(int period) {
        ayumi_set_envelope(&*ayumi, period);
    }

    void SoundGenerator::setEnvelopeShape(int shape) {
        static int lastShape = -1;
        if (shape == lastShape) {
            return;
        }
        ayumi_set_envelope_shape(&*ayumi, shape);
        lastShape = shape;
    }

    int SoundGenerator::getClockRate() const {
        return clockRate;
    }

    void SoundGenerator::setGain(float gain) {
        this->gain = gain;
    }

    void SoundGenerator::process(float* left, float* right, const uint32_t size) {
        for (int i = 0; i < size; i++) {
            ayumi_process(&*ayumi);
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
