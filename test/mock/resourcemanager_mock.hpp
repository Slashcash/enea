#ifndef RESOURCEMANAGERMOCK_HPP
#define RESOURCEMANAGERMOCK_HPP

#include "resourcemanager.hpp"

#include <SFML/Graphics/Font.hpp>
#include <gmock/gmock.h>

class ResourceManagerMock : public ResourceManager<sf::Font>
{
 public:
    using ResourceManager<sf::Font>::ResourceManager;

    MOCK_METHOD(ResourceManager::MemoryRegion, loadFromFile, (const std::filesystem::path& path), (const override));
    MOCK_METHOD(std::optional<sf::Font>, loadFromMemory, (const ResourceManager::MemoryRegion& memory),
                (const override));
};

#endif // INPUTMANAGERMOCK_HPP
