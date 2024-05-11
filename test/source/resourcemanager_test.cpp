#include "resourcemanager_mock.hpp"

const std::filesystem::path RESOURCE_PATH = std::filesystem::absolute("test");

class ResourceManagerFixture : public ::testing::Test
{
 protected:
    ResourceManagerMock resourcemanager;
};

TEST_F(ResourceManagerFixture, get)
{
    // Throwing if invalid file
    EXPECT_CALL(resourcemanager, loadFromFile(RESOURCE_PATH))
        .WillOnce(testing::Return(ResourceManager<sf::Font>::MemoryRegion{nullptr, 0}));
    EXPECT_CALL(resourcemanager, loadFromMemory(::testing::_)).WillOnce(testing::Return(std::nullopt));
    EXPECT_THROW(auto res = resourcemanager.getResource(RESOURCE_PATH), ResourceManagerMock::Excep);

    // Correctly loading file
    EXPECT_CALL(resourcemanager, loadFromFile(RESOURCE_PATH))
        .WillOnce(testing::Return(ResourceManager<sf::Font>::MemoryRegion{nullptr, 0}));
    EXPECT_CALL(resourcemanager, loadFromMemory(::testing::_)).WillOnce(testing::Return(sf::Font()));
    EXPECT_NO_THROW(auto res = resourcemanager.getResource(RESOURCE_PATH));

    // If already loaded we don't load from disk
    EXPECT_CALL(resourcemanager, loadFromFile(::testing::_)).Times(0);
    EXPECT_CALL(resourcemanager, loadFromMemory(::testing::_)).Times(0);
    EXPECT_NO_THROW(auto res = resourcemanager.getResource(RESOURCE_PATH));
}
