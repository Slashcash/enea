#include "cache_mock.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "softwareinfo.hpp"

static const std::filesystem::path CACHE_FILE = std::filesystem::absolute("cache.json");
static const std::filesystem::path SOURCE_FOLDER = std::filesystem::absolute("romfolder");
static const std::string LAST_EDIT = "1970-01-01 00:00:00";

static const std::string VALID_JSON =
    fmt::format(R"(
{{
    "version": "{}",
    "romSources":
    {{
        "path": "{}",
        "lastModified": "{}",
        "roms": [
        {{
            "path": "{}/avsp.zip"
        }}]
    }}
}}
)",
                projectVersion, SOURCE_FOLDER.string(), LAST_EDIT, SOURCE_FOLDER.string());

static const std::string WRONG_VERSION_JSON = fmt::format(R"(
{{
    "version": "wrongVersion",
    "romSources":
    {{
        "path": "{}",
        "lastModified": "{}",
        "roms": [
        {{
            "path": "{}/avsp.zip"
        }}]
    }}
}}
)",
                                                          SOURCE_FOLDER.string(), LAST_EDIT, SOURCE_FOLDER.string());

class CacheFixture : public ::testing::Test
{
 protected:
    CacheMock cache{CACHE_FILE};
};

TEST_F(CacheFixture, loadFromFile)
{
    EXPECT_EQ(cache.path(), CACHE_FILE);
    EXPECT_FALSE(cache.loaded());
    EXPECT_FALSE(cache.roms().has_value());
    EXPECT_FALSE(cache.lastEditTime().has_value());

    EXPECT_CALL(cache, loadCacheFile(CACHE_FILE)).WillOnce(testing::Return(VALID_JSON));
    ASSERT_FALSE(cache.load().has_value());
    auto roms = cache.roms();
    ASSERT_TRUE(roms.has_value());
    EXPECT_EQ(roms->size(), 1);
    ASSERT_TRUE(cache.lastEditTime().has_value());
    EXPECT_EQ(*(cache.lastEditTime()), LAST_EDIT);
    EXPECT_TRUE(cache.loaded());
}

TEST_F(CacheFixture, loadWrongVersion)
{
    EXPECT_CALL(cache, loadCacheFile(CACHE_FILE)).WillOnce(testing::Return(WRONG_VERSION_JSON));
    auto error = cache.load();
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(*error, Cache::Error::INCOMPATIBLE_CACHE_VERSION);
    EXPECT_FALSE(cache.roms().has_value());
    EXPECT_FALSE(cache.lastEditTime().has_value());
    EXPECT_FALSE(cache.loaded());
}

TEST_F(CacheFixture, loadInvalidJson)
{
    EXPECT_CALL(cache, loadCacheFile(CACHE_FILE)).WillOnce(testing::Return("This is not a valid json"));
    auto error = cache.load();
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(*error, Cache::Error::UNABLE_TO_PARSE_CACHE);
    EXPECT_FALSE(cache.roms().has_value());
    EXPECT_FALSE(cache.lastEditTime().has_value());
    EXPECT_FALSE(cache.loaded());
}

TEST_F(CacheFixture, loadFromInfo)
{
    std::list<Rom> roms;
    roms.emplace_back("/sf2.zip");
    cache.setRomCache(SOURCE_FOLDER, roms, LAST_EDIT);
    auto cacheRoms = cache.roms();
    ASSERT_TRUE(cacheRoms.has_value());
    EXPECT_EQ(cacheRoms->size(), 1);
    ASSERT_TRUE(cache.lastEditTime().has_value());
    EXPECT_EQ(*(cache.lastEditTime()), LAST_EDIT);
    EXPECT_TRUE(cache.loaded());
}
