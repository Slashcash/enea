#ifndef EXTERNALRESOURCEMANAGER_HPP
#define EXTERNALRESOURCEMANAGER_HPP

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <SFML/Graphics/Texture.hpp>

#include "resourcemanager.hpp"

template <SFMLLoadable T> class ExternalResourceManager : public ResourceManager<T>
{
 private:
    mutable std::vector<char> buffer;

    ExternalResourceManager() = default;

    [[nodiscard]] inline typename ResourceManager<T>::MemoryRegion
    loadFromFile(const std::filesystem::path& path) const override
    {
        std::ifstream file(path.string());
        if (!file.is_open())
        {
            return {nullptr, 0};
        }

        std::ostringstream stringstream;
        stringstream << file.rdbuf();
        const auto& string = stringstream.str();
        buffer.clear();
        buffer = std::vector<char>(string.begin(), string.end());
        return {buffer.data(), buffer.size()};
    }

 public:
    [[nodiscard]] inline static ExternalResourceManager& get()
    {
        static ExternalResourceManager manager;
        return manager;
    }
};

using ScreenShotManager = ExternalResourceManager<sf::Texture>;

#endif // EXTERNALRESOURCEMANAGER_HPP
