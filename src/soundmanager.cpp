#include "soundmanager.hpp"

void SoundManager::play(const fs::path& thePath) {
    auto it = sbuffermap.find(thePath); //search for the soundbuffer
    if( it != sbuffermap.end() ) { soundvector.push_back(sf::Sound(it->second)); soundvector.back().play();  } //if already loaded, play it
    else { soundvector.push_back(sf::Sound(loadBuffer(thePath))); soundvector.back().play(); } //load it otherwise

    //garbage collection
    garbagecollector_count++;
    if( garbagecollector_count == garbagecollector_max ) {
        garbageCollector();
        garbagecollector_count = 0;
    }
}

const sf::SoundBuffer& SoundManager::loadBuffer(const fs::path& thePath) {
    sf::SoundBuffer buffer;
    buffer.loadFromFile(thePath); //effectively load the sound
    return sbuffermap.insert(std::make_pair(thePath, buffer)).first->second; //store it
}

void SoundManager::garbageCollector() {
    for( auto it = soundvector.begin(); it < soundvector.end(); it++ )
        if( (*it).getStatus() == sf::Sound::Status::Stopped )
            soundvector.erase(it);
}

void SoundManager::loadSounds(const std::vector<fs::path>& thePaths) {
    for( auto it = thePaths.begin(); it < thePaths.end(); it++ )
        loadBuffer(*it);
}

void SoundManager::onNotify(const Signal& theSignal) {
    for( auto it = soundcommandvector.begin(); it < soundcommandvector.end(); it++ )
        if( (*it).first.getName() == theSignal.getName() )
            play((*it).second);

}
