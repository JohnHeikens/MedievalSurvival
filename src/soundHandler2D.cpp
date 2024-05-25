#include "soundHandler2D.h"
#include "filesystem/sfmlInputStream.h"
#include "filesystem/fileio.h"
#include <SFML/Audio/Listener.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

int playingSoundCount = 0;
constexpr int maxSources = 0x100;

void soundHandler2d::update(cvec3 &earPosition, cfp &hearingRange, cfp &maxVolume) {
    cfp hearingRange3d = cvec2(hearingRange, earPosition.z).length();
    sf::Listener::setPosition((float) earPosition.x, (float) earPosition.y, (float) earPosition.z);
    sf::Listener::setDirection(0.0f, 0.0f, -1.0f); //-z is at the back of the screen
    sf::Listener::setUpVector(0.0f, 1.0f, 0.0f);
    sf::Listener::setGlobalVolume((float) maxVolume * 100.0f);
    cmicroseconds currentTime = getmicroseconds();
    currentlyPlayIngAudio.update();
    cfp minimalVolume = 0.05f, maximalVolume = 1.0f;
    cfp distanceFullVolume3D = hearingRange3d * minimalVolume;

    for (size_t i = 0; i < currentlyPlayIngAudio.size; i++) {
        std::shared_ptr<audio2d> s = currentlyPlayIngAudio[i];

        cfp distance3D = (vec3(s->pos) - earPosition).length();
        cbool canHear = (distance3D <= hearingRange3d) || (!s->isSpatial);
        cfp volumeMultiplier = math::minimum(distanceFullVolume3D / distance3D, maximalVolume);

        if (s->startedPlaying) {
            //const microseconds soundDuration = 1000000;
            // check if stopped
            if (s->audioLoaded()) {
                if (s->getStatus() == sf::SoundSource::Status::Stopped) {
                    // remove from list
                    playingSoundCount--;
                    currentlyPlayIngAudio.erase(i);
                    continue;
                }
            } else {
                const microseconds soundDuration = s->getDuration();
                if (s->startedPlaying + soundDuration < currentTime) {
                    currentlyPlayIngAudio.erase(i);
                    continue;
                }
            }
        } else {
            s->startedPlaying = currentTime;
        }
        // check position
        if (s->audioLoaded() && (s->getStatus() == sf::SoundSource::Status::Playing)) {
            if (!canHear) {
                playingSoundCount--;
                s->unLoadAudio();
                continue;
            }
        } else {
            // half the maximum
            if (canHear && (playingSoundCount < maxSources)) {
                playingSoundCount++;
                if (!s->audioLoaded()) {
                    s->loadAudio();
                }
                s->play();
                s->setMinDistance(distanceFullVolume3D);
                s->setAttenuation(1.0 / distanceFullVolume3D);
                s->setPlayingOffset(currentTime - s->startedPlaying);
            } else {
            }
        }
        if (canHear) {
            // s->playingSound->setVolume(s->volume * volumeMultiplier * 100);
        }
    }
}

void soundHandler2d::playAudio(const std::shared_ptr<audio2d> &audioToPlay) {
    currentlyPlayIngAudio.push_back(audioToPlay);
}

void soundHandler2d::stopAudio(const std::shared_ptr<audio2d> &audioToStop) {
    //this way, the handler will do all the logic, like calculating how many sounds are playing and unloading the audio
    audioToStop->stop();
}

void sound2d::loadAudio() {
    playingAudio = new sf::Sound(*buffer);
}

microseconds sound2d::getDuration() {
    if (!buffer) {
        throw "buffer not found";
    }
    return buffer->getDuration().asMicroseconds();
}

void music2d::loadAudio() {
    playingAudio = new sf::Music();
    // this way, even on android we can read from an actual file instead of the internal APK storage
    stream = new sfmlInputStream(std::make_shared<std::ifstream>(path, getOpenMode(false)));
    if (!playingAudio->openFromStream(*stream)) {
        throw "file not found";
    }
}

microseconds music2d::getDuration() {
    if (!audioLoaded()) {
        loadAudio();
    }
    return playingAudio->getDuration().asMicroseconds();
}

music2d::~music2d() {
    //the audio has to be deleted first, as it depends on the stream
    delete playingAudio;
    playingAudio = nullptr;
    delete stream;
}

void audio2d::setPosition(cvec2 &newPosition) {
    pos = newPosition;
}
