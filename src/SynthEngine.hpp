#pragma once

#include <cstdint>
#include <vector>
#include "SoundGenerator.hpp"
#include "Channel.hpp"

namespace AyMidi {

    typedef enum {
        MIDI_MSG_NOTE_OFF         = 0x80, /* Note Off */
        MIDI_MSG_NOTE_ON          = 0x90, /* Note On */
        MIDI_MSG_KEY_PRESSURE     = 0xA0, /* Polyphonic Key Pressure */
        MIDI_MSG_CONTROL          = 0xB0, /* Control Change */
        MIDI_MSG_PGM_CHANGE       = 0xC0, /* Program Change */
        MIDI_MSG_CHANNEL_PRESSURE = 0xD0, /* Channel Pressure (After-touch) */
        MIDI_MSG_PITCH_BEND       = 0xE0, /* Pitch Bend Change */
        MIDI_MSG_SYSTEM_EXCLUSIVE = 0xF0, /* System Exclusive */
        MIDI_MSG_MTC_QUARTER      = 0xF1, /* Midi Time Code Quarter Frame */
        MIDI_MSG_SONG_POS         = 0xF2, /* Song Position Pointer */
        MIDI_MSG_SONG_SELECT      = 0xF3, /* Song Select */
        MIDI_MSG_TUNE_REQUEST     = 0xF6, /* Tune Request */
        MIDI_MSG_END_EXCLUSIVE    = 0xF7, /* End of Exclusive */
        MIDI_MSG_CLOCK            = 0xF8, /* Timing Clock */
        MIDI_MSG_START            = 0xFA, /* Start */
        MIDI_MSG_CONTINUE         = 0xFB, /* Continue */
        MIDI_MSG_STOP             = 0xFC, /* Stop */
        MIDI_MSG_ACTIVE_SENSING   = 0xFE, /* Active Sensing */
        MIDI_MSG_RESET            = 0xFF  /* Reset */
    } MidiMsgStatus;

    typedef enum {
        MIDI_CTL_MSB_BANK             = 0x00, /* Bank Select */
        MIDI_CTL_MSB_MODWHEEL         = 0x01, /* Modulation Wheel or Lever */
        MIDI_CTL_MSB_BREATH           = 0x02, /* Breath Controller */
        MIDI_CTL_MSB_FOOT             = 0x04, /* Foot Controller */
        MIDI_CTL_MSB_PORTAMENTO_TIME  = 0x05, /* Portamento Time */
        MIDI_CTL_MSB_DATA_ENTRY       = 0x06, /* Data Entry MSB */
        MIDI_CTL_MSB_MAIN_VOLUME      = 0x07, /* Channel Volume */
        MIDI_CTL_MSB_BALANCE          = 0x08, /* Balance */
        MIDI_CTL_MSB_PAN              = 0x0A, /* Pan */
        MIDI_CTL_MSB_EXPRESSION       = 0x0B, /* Expression Controller */
        MIDI_CTL_MSB_EFFECT1          = 0x0C, /* Effect Control 1 */
        MIDI_CTL_MSB_EFFECT2          = 0x0D, /* Effect Control 2 */
        MIDI_CTL_MSB_GENERAL_PURPOSE1 = 0x10, /* General Purpose Controller 1 */
        MIDI_CTL_MSB_GENERAL_PURPOSE2 = 0x11, /* General Purpose Controller 2 */
        MIDI_CTL_MSB_GENERAL_PURPOSE3 = 0x12, /* General Purpose Controller 3 */
        MIDI_CTL_MSB_GENERAL_PURPOSE4 = 0x13, /* General Purpose Controller 4 */
        MIDI_CTL_LSB_BANK             = 0x20, /* LSB for Control 0 (Bank Select) */
        MIDI_CTL_LSB_MODWHEEL         = 0x21, /* LSB for Control 1 (Modulation Wheel or Lever) */
        MIDI_CTL_LSB_BREATH           = 0x22, /* LSB for Control 2 (Breath Controller) */
        MIDI_CTL_LSB_FOOT             = 0x24, /* LSB for Control 4 (Foot Controller) */
        MIDI_CTL_LSB_PORTAMENTO_TIME  = 0x25, /* LSB for Control 5 (Portamento Time) */
        MIDI_CTL_LSB_DATA_ENTRY       = 0x26, /* LSB for Control 6 (Data Entry MSB) */
        MIDI_CTL_LSB_MAIN_VOLUME      = 0x27, /* LSB for Control 7 (Channel Volume) */
        MIDI_CTL_LSB_BALANCE          = 0x28, /* LSB for Control 8 (Balance) */
        MIDI_CTL_LSB_PAN              = 0x2A, /* LSB for Control 10 (Pan) */
        MIDI_CTL_LSB_EXPRESSION       = 0x2B, /* LSB for Control 11 (Expression Controller) */
        MIDI_CTL_LSB_EFFECT1          = 0x2C, /* LSB for Control 12 (Effect Control 1) */
        MIDI_CTL_LSB_EFFECT2          = 0x2D, /* LSB for Control 13 (Effect Control 2) */
        MIDI_CTL_LSB_GENERAL_PURPOSE1 = 0x30, /* LSB for Control 16 (General Purpose Controller 1) */
        MIDI_CTL_LSB_GENERAL_PURPOSE2 = 0x31, /* LSB for Control 17 (General Purpose Controller 2) */
        MIDI_CTL_LSB_GENERAL_PURPOSE3 = 0x32, /* LSB for Control 18 (General Purpose Controller 3) */
        MIDI_CTL_LSB_GENERAL_PURPOSE4 = 0x33, /* LSB for Control 19 (General Purpose Controller 4) */
        MIDI_CTL_SUSTAIN              = 0x40, /* Damper Pedal on/off (Sustain) */
        MIDI_CTL_PORTAMENTO           = 0x41, /* Portamento On/Off*/
        MIDI_CTL_SOSTENUTO            = 0x42, /* Sostenuto On/Off */
        MIDI_CTL_SOFT_PEDAL           = 0x43, /* Soft Pedal On/Off */
        MIDI_CTL_LEGATO_FOOTSWITCH    = 0x44, /* Legato Footswitch */
        MIDI_CTL_HOLD2                = 0x45, /* Hold 2 */
        MIDI_CTL_SC1_SOUND_VARIATION  = 0x46, /* Sound Controller 1 (default: Sound Variation) */
        MIDI_CTL_SC2_TIMBRE           = 0x47, /* Sound Controller 2 (default: Timbre) */
        MIDI_CTL_SC3_RELEASE_TIME     = 0x48, /* Sound Controller 3 (default: Release Time) */
        MIDI_CTL_SC4_ATTACK_TIME      = 0x49, /* Sound Controller 4 (default: Attack Time) */
        MIDI_CTL_SC5_BRIGHTNESS       = 0x4A, /* Sound Controller 5 (default: Brightness) */
        MIDI_CTL_SC6                  = 0x4B, /* Sound Controller 6 (default: Decay Time) */
        MIDI_CTL_SC7                  = 0x4C, /* Sound Controller 7 (default: Vibrato Rate) */
        MIDI_CTL_SC8                  = 0x4D, /* Sound Controller 8 (default: Vibrato Depth) */
        MIDI_CTL_SC9                  = 0x4E, /* Sound Controller 9 (default: Vibrato Delay) */
        MIDI_CTL_SC10                 = 0x4F, /* Sound Controller 10 (default: undefined)  */
        MIDI_CTL_GENERAL_PURPOSE5     = 0x50, /* General Purpose Controller 5 */
        MIDI_CTL_GENERAL_PURPOSE6     = 0x51, /* General Purpose Controller 6 */
        MIDI_CTL_GENERAL_PURPOSE7     = 0x52, /* General Purpose Controller 7 */
        MIDI_CTL_GENERAL_PURPOSE8     = 0x53, /* General Purpose Controller 8 */
        MIDI_CTL_PORTAMENTO_CONTROL   = 0x54, /* Portamento Control */
        MIDI_CTL_LSB_VELOCITY_PREFIX  = 0x58, /* High Resolution Velocity Prefix */
        MIDI_CTL_EFFECTS1_DEPTH       = 0x5B, /* Effects 1 Depth (default: Reverb Send Level) */
        MIDI_CTL_EFFECTS2_DEPTH       = 0x5C, /* Effects 2 Depth (formerly Tremolo Depth) */
        MIDI_CTL_EFFECTS3_DEPTH       = 0x5D, /* Effects 3 Depth (default: Chorus Send Level) */
        MIDI_CTL_EFFECTS4_DEPTH       = 0x5E, /* Effects 4 Depth (formerly Celeste [Detune] Depth) */
        MIDI_CTL_EFFECTS5_DEPTH       = 0x5F, /* Effects 5 Depth (formerly Phaser Depth) */
        MIDI_CTL_DATA_INCREMENT       = 0x60, /* Data Increment */
        MIDI_CTL_DATA_DECREMENT       = 0x61, /* Data Decrement */
        MIDI_CTL_NRPN_LSB             = 0x62, /* Non-registered Parameter Number */
        MIDI_CTL_NRPN_MSB             = 0x63, /* Non-registered Parameter Number */
        MIDI_CTL_RPN_LSB              = 0x64, /* Registered Parameter Number */
        MIDI_CTL_RPN_MSB              = 0x65, /* Registered Parameter Number */
        MIDI_CTL_AY_BUZZER_WAVEFORM   = 0x66, /* AY/YM Buzzer Waveform */
        MIDI_CTL_AY_MIX_BOTH          = 0x67, /* AY/YM Mix both buzzer/square */
        MIDI_CTL_AY_BUZ_SQR_RATIO     = 0x68, /* AY/YM Noise Period */
        MIDI_CTL_AY_BUZ_SQR_DETUNE    = 0x69, /* AY/YM Noise Period */
        MIDI_CTL_AY_NOISE_PERIOD      = 0x6A, /* AY/YM Noise Period */
        MIDI_CTL_AY_ATTACK_PITCH      = 0x6B, /* AY/YM Noise Period */
        MIDI_CTL_AY_ATTACK            = 0x6C, /* AY/YM Noise Period */
        MIDI_CTL_AY_HOLD              = 0x6D, /* AY/YM Noise Period */
        MIDI_CTL_AY_DECAY             = 0x6E, /* AY/YM Noise Period */
        MIDI_CTL_AY_SUSTAIN           = 0x6F, /* AY/YM Noise Period */
        MIDI_CTL_AY_RELEASE           = 0x70, /* AY/YM Noise Period */
        MIDI_CTL_AY_ARPEGGIO_SPEED    = 0x71, /* AY/YM Noise Period */
        MIDI_CTL_ALL_SOUNDS_OFF       = 0x78, /* All Sounds Off */
        MIDI_CTL_RESET_CONTROLLERS    = 0x79, /* Reset All Controllers */
        MIDI_CTL_LOCAL_CONTROL_SWITCH = 0x7A, /* Local Control On/Off */
        MIDI_CTL_ALL_NOTES_OFF        = 0x7B, /* All Notes Off */
        MIDI_CTL_OMNI_MODE_OFF        = 0x7C, /* Omni Mode Off */
        MIDI_CTL_OMNI_MODE_ON         = 0x7D, /* Omni Mode On */
        MIDI_CTL_MONO_MODE_ON         = 0x7E, /* Mono Mode On */
        MIDI_CTL_POLY_MODE_ON         = 0x7F  /* Poly Mode On */
    } MidiControl;

    class SynthEngine {
        private:
            std::shared_ptr<SoundGenerator> sg;
            std::shared_ptr<Channel> channels[16];
            std::shared_ptr<Voice> voices[3];
            std::vector<int> voicePool;
            int updateRate;
            int updatePeriod;
            int updateCounter;
            int basicChannel;
            int lastChannel;
            bool omniMode;
            bool polyMode;

            int getLevel(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const;
            int freqToTonePeriod(const double freq) const;
            int freqToBuzzerPeriod(const double freq) const;
            int getTonePeriod(const double note) const;
            int getBuzzerPeriod(const double note) const;
            int getTonePeriod(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const;
            int getTonePeriod(const int buzzerPeriod, const std::shared_ptr<Channel> channel) const;
            float buzzerPeriodMult(const std::shared_ptr<Channel> channel) const;
            int getBuzzerPeriod(const std::shared_ptr<Voice> voice, const std::shared_ptr<Channel> channel) const;
            int getBuzzerPeriod(const int tonePeriod, const std::shared_ptr<Channel> channel) const;
            void updateEnvelope(std::shared_ptr<Voice> voice, std::shared_ptr<Channel> channel);
            int spreadInt(const int value, const int bits, const int max) const;
            float unsignedFloat(int value, int bits) const;
            float signedFloat(int value, int bits) const;
            void synch();
            MidiMsgStatus getMidiMsgStatus(const uint8_t* msg);
            void allNotesOff();
            void noteOn(Channel* channel, int note, int velocity);

        public:
            SynthEngine(std::shared_ptr<SoundGenerator> sg);
            void process(float *left, float *right, const uint32_t size);
            void midiSend(const uint8_t* message);
            void setNoisePeriod(const int index, const int period);
            void setMultRatio(const int index, const int ratio);
            void setMultDetune(const int index, const int detune);
            void setArpeggioSpeed(const int index, const int speed);
            void setAttackPitch(const int index, const int pitch);
            void setAttack(const int index, const int attack);
            void setHold(const int index, const int hold);
            void setDecay(const int index, const int decay);
            void setSustain(const int index, const float sustain);
            void setRelease(const int index, const int release);
            void setUpdateRate(int rate);
            void setBasicChannel(int nChannel);
            void setOmniMode(bool enable);
            void setPolyMode(bool enable);
            void setHarpMode(bool enable);
    };

}
