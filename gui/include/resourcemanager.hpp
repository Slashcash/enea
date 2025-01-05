#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <filesystem>
#include <map>
#include <optional>

#include <cmrc/cmrc.hpp>
#include <fmt/core.h>

#include "exception.hpp"

CMRC_DECLARE(resources);

template <typename F>
concept SFMLLoadable = requires(F resource, const void* data, std::size_t size) {
    requires std::is_default_constructible_v<F>;
    { resource.loadFromMemory(data, size) } -> std::same_as<bool>;
};

template <SFMLLoadable T> class ResourceManager
{
 public:
    struct MemoryRegion
    {
        const char* data;
        long long unsigned int size;
    };

 private:
    std::map<const std::filesystem::path, const T> mResourceMap;

    [[nodiscard]] virtual MemoryRegion loadFromFile(const std::filesystem::path& path) const = 0;
    [[nodiscard]] virtual inline std::optional<T> loadFromMemory(const MemoryRegion& memory) const
    {
        T resource;
        if (!resource.loadFromMemory(memory.data, memory.size))
        {
            return std::nullopt;
        }

        return resource;
    }

 public:
    class Exception : public enea::Exception
    {
        using enea::Exception::Exception;
    };

    ResourceManager() = default;
    ResourceManager(const ResourceManager& resourcemanager) = delete;
    ResourceManager(ResourceManager&& resourcemanager) = delete;

    [[nodiscard]] inline const T& getResource(const std::filesystem::path& path)
    {
        // Searching for resource in cache
        if (auto resourceInMap = mResourceMap.find(path); resourceInMap != mResourceMap.end())
        {
            return resourceInMap->second;
        }

        // If we don't find it we try to load from file
        if (auto resource = loadFromMemory(loadFromFile(path)); resource)
        {
            return mResourceMap.insert(std::make_pair(path, *resource)).first->second;
        }

        throw ResourceManager::Exception(fmt::format("Cannot load resource {}", path.string()));
    }

    ResourceManager& operator=(const ResourceManager& resourcemanager) = delete;
    ResourceManager& operator=(ResourceManager&& resourcemanager) = delete;

    virtual ~ResourceManager() = default;
};

#endif // RESOURCEMANAGER_HPP
