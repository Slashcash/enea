#ifndef _SOUNDMANAGER_HPP_
#define _SOUNDMANAGER_HPP_

#include <string>
#include <map>
#include <utility>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include "filesystem.hpp"
#include "signal.hpp"
#include "observer.hpp"

typedef std::pair<Signal, fs::path> SoundCommand;

class SoundManager : public Observer {
    private:
        //members
        std::map<const fs::path, const sf::SoundBuffer> sbuffermap; //the map which contains the soundbuffers
        std::vector<sf::Sound> soundvector;
        unsigned int garbagecollector_max; //when _count reaches _max the garbagecollector gets triggered (it can be heavy)
        unsigned int garbagecollector_count;
        std::vector<SoundCommand> soundcommandvector;

        //functions
        const sf::SoundBuffer& loadBuffer(const fs::path& thePath);
        void garbageCollector(); //this function checks for already played sounds in memory to delete them
        virtual void onNotify(const Signal& theSignal);

    public:
        //ctor
        SoundManager() { garbagecollector_max = 100; garbagecollector_count = 0; }
        SoundManager(const std::vector<SoundCommand>& theSoundCommand) : SoundManager() { soundcommandvector = theSoundCommand; }

        //getters
        unsigned int getGarbageCollector() const { return garbagecollector_max; }
        std::vector<SoundCommand> getSoundCommand() const { return soundcommandvector; }

        //setters
        void setGarbageCollector(const unsigned int theValue) { garbagecollector_max = theValue; }
        void setSoundCommand(const std::vector<SoundCommand>& theSoundCommand) { soundcommandvector = theSoundCommand; }

        //various
        void play(const fs::path& thePath); //plays a sound
        void loadSounds(const std::vector<fs::path>& thePaths); //this function loads a large number of soundbuffers in one shit (BE AWARE THIS CAN BE HEAVY)


};



#endif // _SOUNDMANAGER_HPP_
