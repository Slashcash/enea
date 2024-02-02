#include "rom.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

static const std::filesystem::path ROM_FOLDER = "home";
static const std::string ROM_NAME = "sf2";
static const std::string ROM_EXTENSION = ".zip";
static const std::filesystem::path ROM_FILENAME = std::filesystem::path(ROM_NAME + ROM_EXTENSION);
static const std::filesystem::path ROM_PATH = std::filesystem::absolute(ROM_FOLDER) / ROM_FILENAME;

static const std::filesystem::path WRONG_ROM_PATH = std::filesystem::absolute("tmp") / ROM_FILENAME;

static const std::string ROM_TITLE = "Street Fighter II: The World Warrior";
static const std::string ROM_YEAR = "1991";
static const std::string ROM_MANUFACTURER = "Capcom";

static const RomDB::RomInfo ROM_INFO{ROM_TITLE, ROM_YEAR, ROM_MANUFACTURER};

using namespace nlohmann::literals;
using namespace nlohmann::literals;
static const nlohmann::json COMPLETE_JSON{
    {"path", ROM_PATH}, {"title", ROM_TITLE}, {"year", ROM_YEAR}, {"manufacturer", ROM_MANUFACTURER}};
static const nlohmann::json INCOMPLETE_JSON{{"path", ROM_PATH}};
static const nlohmann::json WRONG_JSON{{"error", ROM_PATH}};

class RomFixture : public ::testing::Test
{
 protected:
    Rom rom{ROM_PATH};
    Rom sameRom{ROM_PATH};
    Rom notSameRom{WRONG_ROM_PATH};

    Rom romFromInfo{ROM_PATH, ROM_INFO};
    Rom sameRomFromInfo{ROM_PATH, ROM_INFO};
    Rom notSameRomFromInfo{WRONG_ROM_PATH, ROM_INFO};

    Rom romCompleteJson{COMPLETE_JSON};
    Rom romIncompleteJson{INCOMPLETE_JSON};
};

TEST_F(RomFixture, path)
{
    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_EQ(romFromInfo.path(), ROM_PATH);
    EXPECT_EQ(romCompleteJson.path(), ROM_PATH);
    EXPECT_EQ(romIncompleteJson.path(), ROM_PATH);
}

TEST_F(RomFixture, title)
{
    EXPECT_FALSE(rom.title().has_value());
    ASSERT_TRUE(romCompleteJson.title().has_value());
    EXPECT_EQ(*(romCompleteJson.title()), ROM_TITLE);
    EXPECT_FALSE(romIncompleteJson.title().has_value());
    ASSERT_TRUE(romFromInfo.title().has_value());
    EXPECT_EQ(*(romFromInfo.title()), ROM_TITLE);
}

TEST_F(RomFixture, year)
{
    EXPECT_FALSE(rom.year().has_value());
    ASSERT_TRUE(romCompleteJson.year().has_value());
    EXPECT_EQ(*(romCompleteJson.year()), ROM_YEAR);
    EXPECT_FALSE(romIncompleteJson.year().has_value());
    ASSERT_TRUE(romFromInfo.year().has_value());
    EXPECT_EQ(*(romFromInfo.year()), ROM_YEAR);
}

TEST_F(RomFixture, manufacturer)
{
    EXPECT_FALSE(rom.manufacturer().has_value());
    ASSERT_TRUE(romCompleteJson.manufacturer().has_value());
    EXPECT_EQ(*(romCompleteJson.manufacturer()), ROM_MANUFACTURER);
    EXPECT_FALSE(romIncompleteJson.manufacturer().has_value());
    ASSERT_TRUE(romFromInfo.manufacturer().has_value());
    EXPECT_EQ(*(romFromInfo.manufacturer()), ROM_MANUFACTURER);
}

TEST_F(RomFixture, comparison)
{
    EXPECT_EQ(rom, sameRom);
    EXPECT_NE(rom, notSameRom);

    EXPECT_EQ(romFromInfo, sameRomFromInfo);
    EXPECT_NE(romFromInfo, notSameRomFromInfo);
}

TEST_F(RomFixture, to_json)
{
    EXPECT_EQ(COMPLETE_JSON, nlohmann::json(romCompleteJson));
    EXPECT_EQ(INCOMPLETE_JSON, nlohmann::json(romIncompleteJson));
}

TEST_F(RomFixture, from_invalidJson)
{
    EXPECT_THROW(Rom{WRONG_JSON}, Rom::Exception);
}