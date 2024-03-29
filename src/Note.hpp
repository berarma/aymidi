#pragma once

#include <memory>
#include "types.hpp"
#include "Voice.hpp"

namespace AyMidi {

    class Note {

        private:
            ChannelData* params;
            std::shared_ptr<Voice> voice;
            bool inRelease;
            float envelopeLevel;
            float releaseStartLevel;
            unsigned timeCounter = 0;
            unsigned releaseCounter;
            float envelopePitch;
            bool setup;
            bool released;
            bool valid;
            int startKey;

            float getNoteFreq(const double key) const;
            int getLevel() const;
            int getSquareFreq(int updateRate) const;
            int getBuzzerFreq(int updateRate) const;

        public:
            int channelId;
            int key;
            int velocity;
            int pressure;

            Note(ChannelData* params, int key, int velocity, int channelId);
            void setVoice(std::shared_ptr<Voice> voice);
            void release();
            void drop();
            bool isValid();
            bool isReleased();
            void setPressure(int pressure);
            void setStartKey(int key);
            void updateEnvelope();
            void update(int updateRate);
    };
}
