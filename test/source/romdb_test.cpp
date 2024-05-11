#include "romdb_mock.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

class RomDBFixture : public ::testing::Test
{
 protected:
    RomDBMock romdb;
};

static const std::string ROM_NAME = "sf2";
static const std::string ROM_TITLE = "Street Fighter II - The World Warrior (World 910522)";
static const std::string ROM_YEAR = "1991";
static const std::string ROM_MANUFACTURER = "Capcom";
static const std::string INCOMPLETE_ROM_NAME = "avsp";
static const std::string BIOS_NAME = "hng64";
static const std::string BIOS_TITLE = "Hyper NeoGeo 64 Bios";
static const std::string BIOS_YEAR = "1997";
static const std::string BIOS_MANUFACTURER = "SNK";

static const std::string XML = fmt::format(R"(
<datafile>
	<header>
		<name>MAME</name>
	</header>
    <game name="{}" sourcefile="cps1.c">
		<description>{}</description>
		<year>{}</year>
		<manufacturer>{}</manufacturer>
	</game>
	<game name="{}" sourcefile="cps1.c">
	</game>
    <game name="{}" sourcefile="hng64.c" isbios="yes">
        <description>{}</description>
        <year>{}</year>
        <manufacturer>{}</manufacturer>
    </game>
</datafile>
)",
                                           ROM_NAME, ROM_TITLE, ROM_YEAR, ROM_MANUFACTURER, INCOMPLETE_ROM_NAME,
                                           BIOS_NAME, BIOS_TITLE, BIOS_YEAR, BIOS_MANUFACTURER);

static const std::string FORMALLY_VALID_XML = R"(
<datafile>
	<header>
		<name>MAME</name>
	</header>
</datafile>
)";

TEST_F(RomDBFixture, load)
{
    EXPECT_FALSE(romdb.loaded());
    ASSERT_FALSE(romdb.find(ROM_NAME).has_value());

    EXPECT_CALL(romdb, loadFromFile()).WillOnce(testing::Return(XML));

    // Loading from a valid xml and see if it is succesful
    EXPECT_FALSE(romdb.load().has_value());
    EXPECT_TRUE(romdb.loaded());

    // Getting info about a valid rom, we should get valid info
    auto infoComplete = romdb.find(std::filesystem::absolute(ROM_NAME + ".zip"));
    ASSERT_TRUE(infoComplete.has_value());
    ASSERT_TRUE(infoComplete->title.has_value());
    ASSERT_TRUE(infoComplete->manufacturer.has_value());
    ASSERT_TRUE(infoComplete->year.has_value());
    ASSERT_TRUE(infoComplete->isBios.has_value());
    EXPECT_EQ(*(infoComplete->title), ROM_TITLE);
    EXPECT_EQ(*(infoComplete->manufacturer), ROM_MANUFACTURER);
    EXPECT_EQ(*(infoComplete->year), ROM_YEAR);
    EXPECT_FALSE(*(infoComplete->isBios));

    // Getting info about rom with incomplete info, we should get incomplete info from the database
    auto infoIncomplete = romdb.find(std::filesystem::absolute(INCOMPLETE_ROM_NAME + ".zip"));
    ASSERT_TRUE(infoIncomplete.has_value());
    ASSERT_FALSE(infoIncomplete->title.has_value());
    ASSERT_FALSE(infoIncomplete->manufacturer.has_value());
    ASSERT_FALSE(infoIncomplete->year.has_value());
    ASSERT_TRUE(infoComplete->isBios.has_value());
    EXPECT_FALSE(*(infoComplete->isBios));

    // Getting info about a bios, we should get valid bios info
    auto infoBios = romdb.find(std::filesystem::absolute(BIOS_NAME + ".zip"));
    ASSERT_TRUE(infoBios.has_value());
    ASSERT_TRUE(infoBios->title.has_value());
    ASSERT_TRUE(infoBios->manufacturer.has_value());
    ASSERT_TRUE(infoBios->year.has_value());
    ASSERT_TRUE(infoBios->isBios.has_value());
    EXPECT_EQ(*(infoBios->title), BIOS_TITLE);
    EXPECT_EQ(*(infoBios->manufacturer), BIOS_MANUFACTURER);
    EXPECT_EQ(*(infoBios->year), BIOS_YEAR);
    EXPECT_TRUE(*(infoBios->isBios));

    // Getting info about a non-existant rom, we should get no info
    auto infoNonExistant = romdb.find(std::filesystem::absolute("lol.zip"));
    ASSERT_FALSE(infoNonExistant.has_value());
}

TEST_F(RomDBFixture, doubleLoad)
{
    EXPECT_CALL(romdb, loadFromFile()).Times(1).WillOnce(testing::Return(XML));

    // We try to load twice, we expect to get correct infos twice but only read from disk once
    for (unsigned int i = 0; i < 2; i++)
    {
        EXPECT_FALSE(romdb.load().has_value());
        EXPECT_TRUE(romdb.loaded());

        // Getting info about a valid rom, we should get valid info
        auto infoComplete = romdb.find(std::filesystem::absolute(ROM_NAME + ".zip"));
        ASSERT_TRUE(infoComplete.has_value());
        ASSERT_TRUE(infoComplete->title.has_value());
        ASSERT_TRUE(infoComplete->manufacturer.has_value());
        ASSERT_TRUE(infoComplete->year.has_value());
        ASSERT_TRUE(infoComplete->isBios.has_value());
        EXPECT_EQ(*(infoComplete->title), ROM_TITLE);
        EXPECT_EQ(*(infoComplete->manufacturer), ROM_MANUFACTURER);
        EXPECT_EQ(*(infoComplete->year), ROM_YEAR);
        EXPECT_FALSE(*(infoComplete->isBios));
    }
}

TEST_F(RomDBFixture, loadError)
{
    EXPECT_CALL(romdb, loadFromFile()).WillOnce(testing::Return(""));

    // Loading an empy file / file not found, we should not get any rom info
    auto error = romdb.load();
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(*error, RomDB::Error::UNABLE_TO_PARSE_DB_FILE);
    EXPECT_FALSE(romdb.loaded());
    auto info = romdb.find(std::filesystem::absolute(ROM_NAME + ".zip"));
    EXPECT_FALSE(info.has_value());
}

TEST_F(RomDBFixture, loadUnableToParse)
{
    EXPECT_CALL(romdb, loadFromFile()).WillOnce(testing::Return("This should not be a valid xml!"));

    // Loading an unparsable xml, we should not get any rom info
    auto error = romdb.load();
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(*error, RomDB::Error::UNABLE_TO_PARSE_DB_FILE);
    EXPECT_FALSE(romdb.loaded());
    auto info = romdb.find(std::filesystem::absolute(ROM_NAME + ".zip"));
    EXPECT_FALSE(info.has_value());
}

TEST_F(RomDBFixture, loadFormallyValidXML)
{
    EXPECT_CALL(romdb, loadFromFile()).WillOnce(testing::Return(FORMALLY_VALID_XML));

    // Loading an formally valid xml but not a rom database
    auto error = romdb.load();
    ASSERT_TRUE(error.has_value());
    EXPECT_EQ(*error, RomDB::Error::UNABLE_TO_PARSE_DB_FILE);
    EXPECT_FALSE(romdb.loaded());
    auto info = romdb.find(std::filesystem::absolute(ROM_NAME + ".zip"));
    EXPECT_FALSE(info.has_value());
}
