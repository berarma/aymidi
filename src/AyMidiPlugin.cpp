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
