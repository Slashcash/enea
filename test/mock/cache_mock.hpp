#ifndef CACHEMOCK_HPP
#define CACHEMOCK_HPP

#include "cache.hpp"

#include <gmock/gmock.h>

class CacheMock : public Cache
{
 public:
    using Cache::Cache;

    MOCK_METHOD(std::string, loadCacheFile, (const std::filesystem::path& path), (const override));
    MOCK_METHOD(std::optional<Error>, writeCacheFile, (const std::filesystem::path& path, const std::string& content),
                (const override));
};

#endif // CACHEMOCK_HPP
