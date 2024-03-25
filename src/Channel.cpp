#include <algorithm>
#include <math.h>
#include "Channel.hpp"
#include <DistrhoUtils.hpp>

namespace AyMidi {

    Channel::Channel(std::shared_ptr<VoiceProcessor> vp, int index) :
        index(index),
        vp(vp)
    {
        msgReset();
    }

    std::shared_ptr<Note> Channel::findNote(const int key) const {
        auto it = std::find_if(notes.begin(), notes.end(), [key](std::shared_ptr<Note> note) { return note->key == key; });
        if (it == notes.end()) {
            return nullptr;
        }
        return *it;
    }

    std::shared_ptr<Note> Channel::nextArpeggioNote() {
        if (notes.empty()) {
            return nullptr;
        }
        auto it = std::upper_bound(notes.begin(), notes.end(), currentNote, [arpeggioPeriod = params.arpeggioPeriod](std::shared_ptr<Note> a, std::shared_ptr<Note> b) {
            return arpeggioPeriod > 0 ? a->key < b->key : a->key > b->key;
        });
        if (it == notes.end()) {
            return notes[0];
        }
        return *it;
    }

    void Channel::purgeNotes() {
        notes.erase(std::remove_if(notes.begin(), notes.end(), [](std::shared_ptr<Note> note) { return !note->isValid(); }), notes.end());
    }

    void Channel::msgNoteOn(const int key, const int velocity) {
        if (findNote(key) != nullptr) {
            return;
        }
        auto note = std::make_shared<Note>(&params, key, velocity, index);
        if (params.arpeggioPeriod != 0) {
            notes.insert(std::upper_bound(notes.begin(), notes.end(), note, [arpeggioPeriod = params.arpeggioPeriod](std::shared_ptr<Note> a, std::shared_ptr<Note> b) {
                return arpeggioPeriod > 0 ? a->key < b->key : a->key > b->key;
            }), note);
        } else {
            notes.push_back(note);
        }
        vp->registerNote(note);
        currentNote = note;
    }

    void Channel::msgNoteOff(const int key, const int velocity) {
        std::for_each(notes.begin(), notes.end(), [key](std::shared_ptr<Note> note) {
            if (note->key == key) {
                note->release();
            }
        });
    }

    void Channel::msgKeyPressure(const int key, const int pressure) {
        auto note = findNote(key);
        if (note != nullptr) {
            note->setPressure(pressure);
        }
    }

    void Channel::msgPressure(int pressure) {
        params.pressure = pressure / 127.0f;
    }

    void Channel::msgProgramChange(int program) {
        if (program < 5) {
            params.buzzer = program > 0;
            params.square = program % 2 == 0;
            params.buzzerWaveform = 4 + 2 * (program / 3);
        }
    }

    void Channel::msgVolume(int volume) {
        params.volume = volume / 127.0f;
    }

    void Channel::msgAllSoundsOff() {
        for (auto note : notes) {
            note->drop();
        }
    }

    void Channel::msgAllNotesOff() {
        for (auto note : notes) {
            note->release();
        }
    }

    void Channel::msgPitchBend(int lsb, int msb) {
        params.pitchBend = makeFloat(lsb + (msb << 7), 14, -1.0f, 1.0f);
    }

    void Channel::msgModWheel(int value) {
        params.modWheel = makeFloat(value, 7, 0.0f, 1.0f);
    }

    void Channel::msgPan(int value) {
        params.pan = makeFloat(value, 7, 0.0f, 1.0f);
    }

    void Channel::msgNoisePeriod(int period) {
        params.noisePeriod = makeInt(period, 7, 0, 32);
    }

    void Channel::msgBuzzerDetune(int detune) {
        params.buzzerDetune = makeFloat(detune, 7, -16.0f, 16.0f);
    }

    void Channel::msgSquareDetune(int detune) {
        params.squareDetune = makeFloat(detune, 7, -16.0f, 16.0f);
    }

    void Channel::msgAttackPitch(int pitch) {
        params.envelope.attackPitch = pitch - 64;
    }

    void Channel::msgAttack(int attack) {
        params.envelope.attack = attack;
    }

    void Channel::msgHold(int hold) {
        params.envelope.hold = hold;
    }

    void Channel::msgDecay(int decay) {
        params.envelope.decay = decay;
    }

    void Channel::msgSustain(int sustain) {
        params.envelope.sustain = sustain / 127.0f;
    }

    void Channel::msgRelease(int release) {
        params.envelope.release = release;
    }

    void Channel::msgArpeggioRate(int rate) {
        auto prevArpeggioPeriod = params.arpeggioPeriod;
        params.arpeggioPeriod = (64 - makeInt(rate, 7, 0, 64)) - (rate < 64 ? 65 : 0);
        if (params.arpeggioPeriod == 32) {
            params.arpeggioPeriod = 0;
        }
        if (params.arpeggioPeriod != 0 && (params.arpeggioPeriod * prevArpeggioPeriod) <= 0) {
            std::sort(notes.begin(), notes.end(), [arpeggioPeriod = params.arpeggioPeriod](std::shared_ptr<Note> a, std::shared_ptr<Note> b) {
                return arpeggioPeriod > 0 ? a->key < b->key : a->key > b->key;
            });
        }
    }

    void Channel::msgVibratoRate(int rate) {
        params.vibratoRate = makeFloat(rate, 7, 0.0f, 10.0f);
    }

    void Channel::msgVibratoDepth(int depth) {
        params.vibratoDepth = makeFloat(depth, 7, 0.0f, 1.0f);
    }

    void Channel::msgVibratoDelay(int delay) {
        params.vibratoDelay = makeInt(delay, 7, 0, 32);
    }

    void Channel::msgPortamento(int portamento) {
        params.portamento = portamento > 63;
    }

    void Channel::msgPortamentoTime(int time) {
        params.portamentoTime = makeInt(time, 7, 1, 32);
    }

    void Channel::msgPortamentoControl(int control) {
        params.portamentoControl = control;
    }

    void Channel::msgReset() {
        msgAllSoundsOff();
        msgResetCC();
        msgProgramChange(0);
        msgVolume(100);
        msgPan(64);
        msgVibratoRate(0);
        msgVibratoDepth(0);
        msgVibratoDelay(0);
        msgNoisePeriod(0);
        msgBuzzerDetune(64);
        msgSquareDetune(64);
        msgArpeggioRate(64);
        msgAttackPitch(0);
        msgAttack(0);
        msgHold(0);
        msgDecay(0);
        msgSustain(127);
        msgRelease(0);
    }

    void Channel::msgResetCC() {
        msgPitchBend(0, 0x40);
        msgModWheel(0);
        msgPressure(0);
        msgPortamento(0);
        msgPortamentoTime(0);
        msgPortamentoControl(0);
    }

    int Channel::makeInt(const int value, const int bits, const int min, const int max) const {
        return std::round(makeFloat(value, bits, min, max));
    }

    float Channel::makeFloat(const int value, const int bits, const float min, const float max) const {
        return (max - min) * value / ((1 << bits) - 1) + min;
    }

    void Channel::updateArpeggio(int updateRate) {
        int arpeggioPeriod = std::round((float)params.arpeggioPeriod * updateRate / 100.0f);
        arpeggioCounter++;
        if (arpeggioCounter >= abs(arpeggioPeriod)) {
            arpeggioCounter = 0;
            auto nextNote = nextArpeggioNote();
            if (nextNote != nullptr) {
                vp->registerNote(nextNote);
                currentNote = nextNote;
            }
        }
    }

    void Channel::update(int updateRate) {
        for (auto note: notes) {
            if (note->isValid()) {
                note->updateEnvelope();
            }
        }
        purgeNotes();
        if (vp->getMonoMode() && params.arpeggioPeriod != 0) {
            updateArpeggio(updateRate);
        }
    }
}
