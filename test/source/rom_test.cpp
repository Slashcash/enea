#include "rom.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

static const std::filesystem::path ROM_PATH = std::filesystem::absolute("sf2.zip");
static const std::filesystem::path DIFFERENT_ROM_PATH = std::filesystem::absolute("avsp.zip");
static const std::filesystem::path BIOS_PATH = std::filesystem::absolute("hng64.zip");
static const std::filesystem::path MEDIA_PATH = std::filesystem::absolute("sf2.png");

static const std::string ROM_TITLE = "Street Fighter II: The World Warrior";
static const std::string ROM_YEAR = "1991";
static const std::string ROM_MANUFACTURER = "Capcom";

static const std::string BIOS_TITLE = "Hyper NeoGeo 64 Bios";
static const std::string BIOS_YEAR = "1997";
static const std::string BIOS_MANUFACTURER = "SNK";

static const RomDB::RomInfo ROM_INFO{ROM_TITLE, ROM_YEAR, ROM_MANUFACTURER, false};
static const RomDB::RomInfo ROM_INCOMPLETE_INFO{.title{ROM_TITLE}};
static const RomDB::RomInfo BIOS_INFO{BIOS_TITLE, BIOS_YEAR, BIOS_MANUFACTURER, true};

using namespace nlohmann::literals;
static const nlohmann::json COMPLETE_JSON{{"path", ROM_PATH},    {"title", ROM_TITLE},
                                          {"year", ROM_YEAR},    {"manufacturer", ROM_MANUFACTURER},
                                          {"media", MEDIA_PATH}, {"isBios", false}};
static const nlohmann::json BIOS_JSON{{"path", BIOS_PATH},
                                      {"title", BIOS_TITLE},
                                      {"year", BIOS_YEAR},
                                      {"manufacturer", BIOS_MANUFACTURER},
                                      {"isBios", true}};
static const nlohmann::json DIFFERENT_JSON{
    {"path", DIFFERENT_ROM_PATH}, {"title", ROM_TITLE}, {"year", ROM_YEAR}, {"manufacturer", ROM_MANUFACTURER}};
static const nlohmann::json INCOMPLETE_JSON{{"path", ROM_PATH}, {"title", ROM_TITLE}};
static const nlohmann::json WRONG_JSON{{"error", ROM_PATH}};

class RomFixture : public ::testing::Test
{
 protected:
    Rom rom{ROM_PATH};
    Rom sameRom{ROM_PATH};
    Rom notSameRom{DIFFERENT_ROM_PATH};

    Rom romFromInfo{ROM_PATH, ROM_INFO};
    Rom biosFromInfo{BIOS_PATH, BIOS_INFO};
    Rom romFromIncompleteInfo{ROM_PATH, ROM_INCOMPLETE_INFO};
    Rom sameRomFromInfo{ROM_PATH, ROM_INFO};
    Rom notSameRomFromInfo{DIFFERENT_ROM_PATH, ROM_INFO};

    Rom romFromJson{COMPLETE_JSON};
    Rom biosFromJson{BIOS_JSON};
    Rom romFromIncompleteJson{INCOMPLETE_JSON};
    Rom sameRomFromJson{COMPLETE_JSON};
    Rom notSameRomFromJson{DIFFERENT_JSON};
};

TEST_F(RomFixture, romFromPath)
{
    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_FALSE(rom.title());
    EXPECT_FALSE(rom.manufacturer());
    EXPECT_FALSE(rom.year());
    EXPECT_FALSE(rom.isBios());
    EXPECT_FALSE(rom.media());
    EXPECT_EQ(rom, sameRom);
    EXPECT_NE(rom, notSameRom);

    rom.setMedia(MEDIA_PATH);
    ASSERT_TRUE(rom.media());
    EXPECT_EQ(*(rom.media()), MEDIA_PATH);
}

TEST_F(RomFixture, romFromInfo)
{
    EXPECT_EQ(romFromInfo.path(), ROM_PATH);
    ASSERT_TRUE(romFromInfo.title().has_value());
    EXPECT_EQ(*(romFromInfo.title()), ROM_TITLE);
    ASSERT_TRUE(romFromInfo.manufacturer().has_value());
    EXPECT_EQ(*(romFromInfo.manufacturer()), ROM_MANUFACTURER);
    ASSERT_TRUE(romFromInfo.year().has_value());
    EXPECT_EQ(*(romFromInfo.year()), ROM_YEAR);
    ASSERT_TRUE(romFromInfo.isBios().has_value());
    EXPECT_FALSE(*(romFromInfo.isBios()));
    EXPECT_FALSE(rom.media());

    EXPECT_EQ(romFromIncompleteInfo.path(), ROM_PATH);
    ASSERT_TRUE(romFromIncompleteInfo.title().has_value());
    EXPECT_EQ(*(romFromIncompleteInfo.title()), ROM_TITLE);
    EXPECT_FALSE(romFromIncompleteInfo.manufacturer().has_value());
    EXPECT_FALSE(romFromIncompleteInfo.year().has_value());
    EXPECT_FALSE(romFromIncompleteInfo.isBios().has_value());
    EXPECT_FALSE(rom.media());

    EXPECT_EQ(biosFromInfo.path(), BIOS_PATH);
    ASSERT_TRUE(biosFromInfo.title().has_value());
    EXPECT_EQ(*(biosFromInfo.title()), BIOS_TITLE);
    ASSERT_TRUE(biosFromInfo.manufacturer().has_value());
    EXPECT_EQ(*(biosFromInfo.manufacturer()), BIOS_MANUFACTURER);
    ASSERT_TRUE(biosFromInfo.year().has_value());
    EXPECT_EQ(*(biosFromInfo.year()), BIOS_YEAR);
    ASSERT_TRUE(biosFromInfo.isBios().has_value());
    EXPECT_TRUE(*(biosFromInfo.isBios()));
    EXPECT_FALSE(rom.media());

    EXPECT_EQ(romFromInfo, sameRomFromInfo);
    EXPECT_NE(romFromInfo, notSameRomFromInfo);
}

TEST_F(RomFixture, romFromJson)
{
    EXPECT_EQ(romFromJson.path(), ROM_PATH);
    ASSERT_TRUE(romFromJson.title().has_value());
    EXPECT_EQ(*(romFromJson.title()), ROM_TITLE);
    ASSERT_TRUE(romFromJson.manufacturer().has_value());
    EXPECT_EQ(*(romFromJson.manufacturer()), ROM_MANUFACTURER);
    ASSERT_TRUE(romFromJson.year().has_value());
    EXPECT_EQ(*(romFromJson.year()), ROM_YEAR);
    ASSERT_TRUE(romFromJson.isBios().has_value());
    EXPECT_FALSE(*(romFromJson.isBios()));
    ASSERT_TRUE(romFromJson.media());
    ASSERT_EQ(*(romFromJson.media()), MEDIA_PATH);

    EXPECT_EQ(biosFromJson.path(), BIOS_PATH);
    ASSERT_TRUE(biosFromJson.title().has_value());
    EXPECT_EQ(*(biosFromJson.title()), BIOS_TITLE);
    ASSERT_TRUE(biosFromJson.manufacturer().has_value());
    EXPECT_EQ(*(biosFromJson.manufacturer()), BIOS_MANUFACTURER);
    ASSERT_TRUE(biosFromJson.year().has_value());
    EXPECT_EQ(*(biosFromJson.year()), BIOS_YEAR);
    ASSERT_TRUE(biosFromJson.isBios().has_value());
    EXPECT_TRUE(*(biosFromJson.isBios()));
    EXPECT_FALSE(biosFromJson.media());

    EXPECT_EQ(romFromIncompleteJson.path(), ROM_PATH);
    ASSERT_TRUE(romFromIncompleteJson.title().has_value());
    EXPECT_EQ(*(romFromIncompleteJson.title()), ROM_TITLE);
    EXPECT_FALSE(romFromIncompleteJson.manufacturer().has_value());
    EXPECT_FALSE(romFromIncompleteJson.year().has_value());
    EXPECT_FALSE(romFromIncompleteJson.isBios().has_value());
    EXPECT_FALSE(romFromIncompleteJson.media());

    EXPECT_THROW(Rom{WRONG_JSON}, Rom::Excep);

    EXPECT_EQ(romFromJson, sameRomFromJson);
    EXPECT_NE(romFromJson, notSameRomFromJson);
}
