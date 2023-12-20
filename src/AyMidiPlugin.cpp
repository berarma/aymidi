#include <cstdint>
#include "DistrhoPlugin.hpp"
#include "SynthEngine.hpp"

START_NAMESPACE_DISTRHO
 
/**
   Our custom plugin class.
   Subclassing `Plugin` from DPF is how this all works.
 
   By default, only information-related functions and `run` are pure virtual (that is, must be reimplemented).
   When enabling certain features (such as programs or states, more on that below), a few extra functions also need to be reimplemented.
 */
class AyMidiPlugin : public Plugin {

    enum ParameterIds {
        GAIN,
        CLOCKRATE,
        EMUL,
        UPDATERATE,
        NOISEPERIOD,
        MULTRATIO,
        MULTDETUNE,
        ARPEGGIOSPEED,
        ATTACKPITCH,
        ATTACK,
        HOLD,
        DECAY,
        SUSTAIN,
        RELEASE,
        RINGMODDEPTH,
        RINGMODDETUNE,
        RINGMODDUTY,
        SYNCBUZZERPERIOD,
        NUM_PARAMETERS
    };

    public:
        /**
          Plugin class constructor.
          */
        AyMidiPlugin()
            : Plugin(NUM_PARAMETERS, 0, 0), // 0 parameters, 0 programs and 0 states
            pGain(1.0),
            pClockRate(2e6),
            pEmul(AyMidi::YM2149)
        {
            soundGenerator = std::make_shared<AyMidi::SoundGenerator>(getSampleRate(), pClockRate);
            synthEngine = std::make_shared<AyMidi::SynthEngine>(soundGenerator);
            soundGenerator->setGain(pGain);
        }

    protected:
        /* ----------------------------------------------------------------------------------------
         * Information */

        /**
          Get the plugin label.
          This label is a short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
          */
        const char* getLabel() const override {
            return "AyMidi";
        }

        /**
          Get the plugin author/maker.
          */
        const char* getMaker() const override {
            return "Bernat Arlandis";
        }

        /**
          Get the plugin license name (a single line of text).
          For commercial plugins this should return some short copyright information.
          */
        const char* getLicense() const override {
            return "GPL2";
        }

        /**
          Get the plugin version, in hexadecimal.
          */
        uint32_t getVersion() const override {
            return d_version(0, 0, 0);
        }

        /**
          Get the plugin unique Id.
          This value is used by LADSPA, DSSI, VST2 and VST3 plugin formats.
          */
        int64_t getUniqueId() const override {
            return d_cconst('A', 'Y', 'M', 'I');
        }

        /**
          Initialize a parameter.
          This function will be called once, shortly after the plugin is created.
          */
        void initParameter(uint32_t index, Parameter& parameter) override
        {
            parameter.hints      = kParameterIsAutomatable;

            switch (index) {
                case GAIN:
                    parameter.name       = "Gain";
                    parameter.symbol     = "Gain";
                    parameter.ranges.min = 0.0f;
                    parameter.ranges.max = 2.0f;
                    parameter.ranges.def = 1.0f;
                    break;
                case CLOCKRATE:
                    parameter.hints     |= kParameterIsInteger;
                    parameter.name       = "Clock Rate";
                    parameter.symbol     = "Clock";
                    parameter.unit       = "Hz";
                    parameter.ranges.min = 1e6;
                    parameter.ranges.max = 2e6;
                    parameter.ranges.def = 2e6;
                    break;
                case EMUL:
                    parameter.hints     |= kParameterIsInteger;
                    parameter.name       = "Emul";
                    parameter.symbol     = "Emul";
                    parameter.ranges.min = 0;
                    parameter.ranges.max = 1;
                    parameter.ranges.def = 1;
                    parameter.enumValues.count = 2;
                    parameter.enumValues.restrictedMode = true;
                    {
                        ParameterEnumerationValue* const enumValues = new ParameterEnumerationValue[2];
                        enumValues[0].value = 0;
                        enumValues[0].label = "AY8910";
                        enumValues[1].value = 1;
                        enumValues[1].label = "YM2149";
                        parameter.enumValues.values = enumValues;
                    }
                    break;
                case UPDATERATE:
                    parameter.hints     |= kParameterIsInteger;
                    parameter.name       = "Update Rate";
                    parameter.symbol     = "UPDRATE";
                    parameter.ranges.min = 10;
                    parameter.ranges.max = 300;
                    parameter.ranges.def = 50;
                    break;
                case NOISEPERIOD:
                    parameter.hints     |= kParameterIsInteger;
                    parameter.name       = "Noise Period";
                    parameter.symbol     = "NOISE";
                    parameter.ranges.min = 0;
                    parameter.ranges.max = 31;
                    parameter.ranges.def = 0;
                    parameter.midiCC     = 70;
                    break;
                case MULTRATIO:
                    parameter.hints     |= kParameterIsInteger;
                    parameter.name       = "Buzzer/Square Ratio";
                    parameter.symbol     = "MRATIO";
                    parameter.ranges.min = 0;
                    parameter.ranges.max = 7;
                    parameter.ranges.def = 0;
                    parameter.midiCC     = 71;
                    break;
                case MULTDETUNE:
                    parameter.hints     |= kParameterIsInteger;
                    parameter.name       = "Buzzer/Square Detune";
                    parameter.symbol     = "MDETUNE";
                    parameter.ranges.min = -64;
                    parameter.ranges.max = 63;
                    parameter.ranges.def = 0;
                    parameter.midiCC     = 72;
                    break;
                case ARPEGGIOSPEED:
                    parameter.name       = "Arpeggio Speed";
                    parameter.symbol     = "ARPEGGIO";
                    parameter.ranges.min = -1.0f;
                    parameter.ranges.max = 1.0f;
                    parameter.ranges.def = 0.0f;
                    parameter.midiCC     = 73;
                    break;
                case ATTACKPITCH:
                    parameter.hints     |= kParameterIsInteger;
                    parameter.name       = "Attack Pitch";
                    parameter.symbol     = "ATKPITCH";
                    parameter.ranges.min = -64;
                    parameter.ranges.max = 63;
                    parameter.ranges.def = 0;
                    parameter.midiCC     = 74;
                    break;
                case ATTACK:
                    parameter.hints     |= kParameterIsInteger;
                    parameter.name       = "Attack";
                    parameter.symbol     = "ATTACK";
                    parameter.ranges.min = 0;
                    parameter.ranges.max = 127;
                    parameter.ranges.def = 0;
                    parameter.midiCC     = 75;
                    break;
                case HOLD:
                    parameter.hints     |= kParameterIsInteger;
                    parameter.name       = "Hold";
                    parameter.symbol     = "HOLD";
                    parameter.ranges.min = 0;
                    parameter.ranges.max = 127;
                    parameter.ranges.def = 0;
                    parameter.midiCC     = 76;
                    break;
                case DECAY:
                    parameter.hints     |= kParameterIsInteger;
                    parameter.name       = "Decay";
                    parameter.symbol     = "DECAY";
                    parameter.ranges.min = 0;
                    parameter.ranges.max = 127;
                    parameter.ranges.def = 0;
                    parameter.midiCC     = 77;
                    break;
                case SUSTAIN:
                    parameter.name       = "Sustain";
                    parameter.symbol     = "SUSTAIN";
                    parameter.ranges.min = 0.0f;
                    parameter.ranges.max = 1.0f;
                    parameter.ranges.def = 1.0f;
                    parameter.midiCC     = 78;
                    break;
                case RELEASE:
                    parameter.hints     |= kParameterIsInteger;
                    parameter.name       = "Release";
                    parameter.symbol     = "RELEASE";
                    parameter.ranges.min = 0;
                    parameter.ranges.max = 127;
                    parameter.ranges.def = 0;
                    parameter.midiCC     = 79;
                    break;
                case RINGMODDEPTH:
                    parameter.name       = "Ringmod Depth";
                    parameter.symbol     = "RINGDEPTH";
                    parameter.ranges.min = 0;
                    parameter.ranges.max = 127;
                    parameter.ranges.def = 0;
                    parameter.midiCC     = 80;
                    break;
                case RINGMODDETUNE:
                    parameter.name       = "Ringmod Detune";
                    parameter.symbol     = "RINGDETUNE";
                    parameter.ranges.min = -1.0f;
                    parameter.ranges.max = 1.0f;
                    parameter.ranges.def = 0.0f;
                    parameter.midiCC     = 81;
                    break;
                case RINGMODDUTY:
                    parameter.name       = "Ringmod Duty";
                    parameter.symbol     = "RINGDUTY";
                    parameter.ranges.min = 0.0f;
                    parameter.ranges.max = 1.0f;
                    parameter.ranges.def = 0.5f;
                    parameter.midiCC     = 82;
                    break;
                case SYNCBUZZERPERIOD:
                    parameter.name       = "SyncBuzzer Period";
                    parameter.symbol     = "SBP";
                    parameter.ranges.min = 0.0f;
                    parameter.ranges.max = 1.0f;
                    parameter.ranges.def = 1.0f;
                    parameter.midiCC     = 83;
                    break;
            }
        }

        /* ----------------------------------------------------------------------------------------
         * Internal data */

        /**
          Get the current value of a parameter.
          */
        float getParameterValue(uint32_t index) const override
        {
            switch (index) {
                case GAIN:
                    return pGain;
                case CLOCKRATE:
                    return pClockRate;
                case EMUL:
                    return pEmul;
                case UPDATERATE:
                    return pUpdateRate;
                case NOISEPERIOD:
                    return pNoisePeriod;
                case MULTRATIO:
                    return pMultRatio;
                case MULTDETUNE:
                    return pMultDetune;
                case ARPEGGIOSPEED:
                    return pArpeggioSpeed;
                case ATTACKPITCH:
                    return pAttackPitch;
                case ATTACK:
                    return pAttack;
                case HOLD:
                    return pHold;
                case DECAY:
                    return pDecay;
                case SUSTAIN:
                    return pSustain;
                case RELEASE:
                    return pRelease;
                case RINGMODDEPTH:
                    return pRingmodDepth;
                case RINGMODDETUNE:
                    return pRingmodDepth;
                case RINGMODDUTY:
                    return pRingmodDuty;
                case SYNCBUZZERPERIOD:
                    return pSyncBuzzerPeriod;
            }

            return 0.0f;
        }

        /**
          Change a parameter value.
          */
        void setParameterValue(uint32_t index, float value) override
        {
            switch (index) {
                case GAIN:
                    pGain = value;
                    soundGenerator->setGain(pGain);
                    break;
                case CLOCKRATE:
                    pClockRate = value;
                    soundGenerator->setClockRate(pClockRate);
                    break;
                case EMUL:
                    pEmul = value;
                    soundGenerator->setEmul(pEmul == 1.0f ? AyMidi::YM2149 : AyMidi::AY8910);
                    break;
                case UPDATERATE:
                    pUpdateRate = value;
                    synthEngine->setUpdateRate((int)pUpdateRate);
                    break;
                case NOISEPERIOD:
                    pNoisePeriod = value;
                    synthEngine->setNoisePeriod(0, (int)pNoisePeriod);
                    break;
                case MULTRATIO:
                    pMultRatio = value;
                    synthEngine->setMultRatio(0, (int)pMultRatio);
                    break;
                case MULTDETUNE:
                    pMultDetune = value;
                    synthEngine->setMultDetune(0, (int)pMultDetune);
                    break;
                case ARPEGGIOSPEED:
                    pArpeggioSpeed = value;
                    synthEngine->setArpeggioSpeed(0, (int)pArpeggioSpeed);
                    break;
                case ATTACKPITCH:
                    pAttackPitch = value;
                    synthEngine->setAttackPitch(0, (int)pAttackPitch);
                    break;
                case ATTACK:
                    pAttack = value;
                    synthEngine->setAttack(0, (int)pAttack);
                    break;
                case HOLD:
                    pHold = value;
                    synthEngine->setHold(0, (int)pHold);
                    break;
                case DECAY:
                    pDecay = value;
                    synthEngine->setDecay(0, (int)pDecay);
                    break;
                case SUSTAIN:
                    pSustain = value;
                    synthEngine->setSustain(0, pSustain);
                    break;
                case RELEASE:
                    pRelease = value;
                    synthEngine->setRelease(0, (int)pRelease);
                    break;
                case RINGMODDEPTH:
                    pRingmodDepth = value;
                    synthEngine->setRingmodDepth(0, pRingmodDepth);
                    break;
                case RINGMODDETUNE:
                    pRingmodDepth = value;
                    synthEngine->setRingmodDetune(0, pRingmodDetune);
                    break;
                case RINGMODDUTY:
                    pRingmodDuty = value;
                    synthEngine->setRingmodDuty(0, pRingmodDuty);
                    break;
                case SYNCBUZZERPERIOD:
                    pSyncBuzzerPeriod = value;
                    synthEngine->setSyncBuzzerPeriod(0, pSyncBuzzerPeriod);
                    break;
            }
        }

        /* ----------------------------------------------------------------------------------------
         * Audio/MIDI Processing */

        /**
          Run/process function for plugins without MIDI input.
          */
        void run(const float**, float** outputs,
                uint32_t frames,
                const MidiEvent* midiEvents,
                uint32_t midiEventCount) override
        {
            float* outL = outputs[0];
            float* outR = outputs[1];

            uint32_t currentFrame = 0;
            for (int i = 0; i < midiEventCount; i++) {
                const MidiEvent& me = midiEvents[i];
                if (me.frame > currentFrame) {
                    const uint32_t size = me.frame - currentFrame;
                    synthEngine->process(outL, outR, size);
                    currentFrame = me.frame;
                    outL += size;
                    outR += size;
                }
                synthEngine->midiSend(me.data);
            }
            synthEngine->process(outL, outR, frames - currentFrame);
        }

    private:
        std::shared_ptr<AyMidi::SoundGenerator> soundGenerator;
        std::shared_ptr<AyMidi::SynthEngine> synthEngine;

        // Parameters
        float pGain;
        float pClockRate;
        float pEmul;
        float pUpdateRate;
        float pNoisePeriod;
        float pMultRatio;
        float pMultDetune;
        float pArpeggioSpeed;
        float pAttackPitch;
        float pAttack;
        float pHold;
        float pDecay;
        float pSustain;
        float pRelease;
        float pRingmodDepth;
        float pRingmodDetune;
        float pRingmodDuty;
        float pSyncBuzzerPeriod;

        DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AyMidiPlugin)
};

/**
  Create an instance of the Plugin class.
  This is the entry point for DPF plugins.
  DPF will call this to either create an instance of your plugin for the host or to fetch some initial information for internal caching.
  */
Plugin* createPlugin() {
    return new AyMidiPlugin();
}

END_NAMESPACE_DISTRHO
