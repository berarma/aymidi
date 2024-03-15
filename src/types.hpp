#pragma once

namespace AyMidi {

    struct Envelope {
        float attackPitch;
        int attack;
        int hold;
        int decay;
        float sustain;
        int release;
    };

    struct ChannelData {
            bool buzzer;
            bool square;
            int buzzerWaveform;
            float pressure;
            float pitchBend;
            float modWheel;
            float pan;
            float volume;
            int noisePeriod;
            float buzzerDetune;
            float squareDetune;
            int arpeggioPeriod;
            Envelope envelope;
    };
}
