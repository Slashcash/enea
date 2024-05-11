#ifndef INTERNALRESOURCEMANAGER_HPP
#define INTERNALRESOURCEMANAGER_HPP

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>

#include "resourcemanager.hpp"

template <SFMLLoadable T> class InternalResourceManager : public ResourceManager<T>
{
 private:
    InternalResourceManager() = default;

    [[nodiscard]] inline typename ResourceManager<T>::MemoryRegion
    loadFromFile(const std::filesystem::path& path) const override
    {
        auto embeddedFS = cmrc::resources::get_filesystem();
        T resource;
        cmrc::file resourceFile;
        try
        {
            resourceFile = embeddedFS.open(path);
        }
        catch ([[maybe_unused]] const std::system_error& exception)
        {
            return {nullptr, 0};
        }

        return {resourceFile.begin(),
                static_cast<long unsigned int>(std::distance(resourceFile.begin(), resourceFile.end()))};
    }

 public:
    [[nodiscard]] inline static InternalResourceManager& get()
    {
        static InternalResourceManager manager;
        return manager;
    }
};

using FontManager = InternalResourceManager<sf::Font>;
using SoundManager = InternalResourceManager<sf::SoundBuffer>;

#endif // INTERNALRESOURCEMANAGER_HPP
