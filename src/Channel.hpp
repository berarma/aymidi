#pragma once

#include <memory>
#include <vector>
#include "types.hpp"
#include "Note.hpp"
#include "VoiceProcessor.hpp"

namespace AyMidi {

    class Channel {

        private:
            int index;
            std::shared_ptr<VoiceProcessor> vp;
            std::vector<std::shared_ptr<Note>> notes;
            std::shared_ptr<Note> currentNote;
            int arpeggioCounter;
            ChannelData params;

            int makeInt(const int value, const int bits, const int min, const int max) const;
            float makeFloat(const int value, const int bits, const float min, const float max) const;
            void updateArpeggio(int updateRate);

        public:
            Channel(std::shared_ptr<VoiceProcessor> vp, int index);
            std::shared_ptr<Note> findNote(const int key) const;
            void purgeNotes();
            std::shared_ptr<Note> nextArpeggioNote();
            void msgNoteOn(int key, int velocity);
            void msgNoteOff(int key, int velocity);
            void msgKeyPressure(const int note, const int pressure);
            void msgPressure(int pressure);
            void msgProgramChange(int newProgram);
            void msgPitchBend(int lsb, int msb);
            void msgVolume(int newVolume);
            void msgPan(int newPan);
            void msgModWheel(int value);
            void msgNoisePeriod(int period);
            void msgBuzzerDetune(int detune);
            void msgSquareDetune(int detune);
            void msgAttackPitch(int pitch);
            void msgAttack(int attack);
            void msgHold(int hold);
            void msgDecay(int decay);
            void msgSustain(int sustain);
            void msgRelease(int release);
            void msgArpeggioRate(int rate);
            void msgAllSoundsOff();
            void msgAllNotesOff();
            void msgReset();
            void msgResetCC();
            void update(int updateRate);
    };
}
