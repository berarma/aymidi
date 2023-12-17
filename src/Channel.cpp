#include "Channel.hpp"
#include "DistrhoUtils.hpp"

namespace AyMidi {

    Channel::Channel(std::shared_ptr<struct ayumi> ayumi, int index) : ayumi(ayumi), index(index) {
        // Ayumi not initialized, don't call ayumi functions yet.
    }

    void Channel::enableTone(bool enable) {
        tone_off = !enable;
        ayumi_set_mixer(&*ayumi, index, tone_off, noise_off, envelope_on);
    }

    void Channel::enableNoise(bool enable) {
        noise_off = !enable;
        ayumi_set_mixer(&*ayumi, index, tone_off, noise_off, envelope_on);
    }

    void Channel::enableEnvelope(bool enable) {
        envelope_on = enable;
        ayumi_set_mixer(&*ayumi, index, tone_off, noise_off, envelope_on);
    }

    void Channel::setLevel(int level) {
        ayumi_set_volume(&*ayumi, index, level);
    }

    void Channel::setTonePeriod(int period) {
        ayumi_set_tone(&*ayumi, index, period);
    }

    void Channel::setPan(float pan) {
        ayumi_set_pan(&*ayumi, index, pan, 1);
    }

}
