#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <filesystem>
#include <map>

#include <SFML/Graphics/Font.hpp>
#include <cmrc/cmrc.hpp>
#include <spdlog/spdlog.h>

CMRC_DECLARE(resources);

template <typename F>
concept Loadable = requires(F resource, const void* data, std::size_t size)
{
    requires std::is_default_constructible_v<F>;
    {
        resource.loadFromMemory(data, size)
        } -> std::same_as<bool>;
};

template <Loadable T> class ResourceManager
{
 private:
    std::map<const std::filesystem::path, const T> mResourceMap;

    [[nodiscard]] inline const T& loadFromFile(const std::filesystem::path& path)
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
            throw ResourceManager::Exception("Failed to load resource from file");
        }

        if (!resource.loadFromMemory(resourceFile.begin(), std::distance(resourceFile.begin(), resourceFile.end())))
        {
            throw ResourceManager::Exception("Failed to load resource from memory");
        }
        return mResourceMap.insert(std::make_pair(path, resource)).first->second;
    }

 public:
    class Exception : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    [[nodiscard]] inline const T& get(const std::filesystem::path& path)
    {
        // Searching for resource in cache
        if (auto resourceInMap = mResourceMap.find(path); resourceInMap != mResourceMap.end())
        {
            return resourceInMap->second;
        }

        // If we don't find it we try to load from file
        else
        {
            try
            {
                return loadFromFile(path);
            }
            // If loading from file fails we try to return a default, otherwise we throw
            catch (const ResourceManager::Exception& exception)
            {
                spdlog::warn("Loading resource file {} failed: {}, will try to load a default one instead",
                             path.string(), exception.what());

                return loadFromFile(path.parent_path() / "default");
            }
        }
    }
};

using FontManager = ResourceManager<sf::Font>;

#endif // RESOURCEMANAGER_HPP