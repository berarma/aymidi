dpf_add_plugin(aymidi
        TARGETS jack lv2 vst2 vst3 clap
        MONOLITHIC
        FILES_DSP
                AyMidiPlugin.cpp
                SynthEngine.cpp
                Channel.cpp
                Note.cpp
                VoiceProcessor.cpp
                SoundGenerator.cpp
                Voice.cpp
                ayumi.c
)

target_include_directories(aymidi PUBLIC
    "."
    "../dpf/distrho"
    "../dpf/dgl"
)
