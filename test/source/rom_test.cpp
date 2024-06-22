#include "rom.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

static const std::filesystem::path ROM_PATH = std::filesystem::absolute("sf2.zip");

static const std::string ROM_TITLE = "Street Fighter II: The World Warrior";
static const std::string ROM_YEAR = "1991";
static const std::string ROM_MANUFACTURER = "Capcom";
static const bool ROM_IS_BIOS = false;

static const std::filesystem::path SCREENSHOT_PATH = std::filesystem::absolute("sf2.png");

static const RomInfo INFO_SET_COMPLETE{
    .title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};
static const RomInfo INFO_SET_TITLE_MISSING{.year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};
static const RomInfo INFO_SET_YEAR_MISSING{.title = ROM_TITLE, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};
static const RomInfo INFO_SET_MANUFACTURER_MISSING{.title = ROM_TITLE, .year = ROM_YEAR, .isBios = ROM_IS_BIOS};
static const RomInfo INFO_SET_BIOS_MISSING{.title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER};
static const RomInfo INFO_SET_EMPTY{};

static const RomMedia MEDIA_SET_COMPLETE{.screenshot = SCREENSHOT_PATH};
static const RomMedia MEDIA_SET_SCREENSHOT_MISSING{};
static const RomMedia MEDIA_SET_EMPTY{};

TEST(Rom, fromJson)
{
    /*
        Building Rom from a json which contains all the information.
        Expectation: Rom is correctly built and every field is correctly set.
    */
    auto rom = Rom{nlohmann::json{{Rom::PATH_JSON_FIELD, ROM_PATH},
                                  {Rom::INFO_JSON_FIELD, INFO_SET_COMPLETE},
                                  {Rom::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.title() && *(rom.title()) == ROM_TITLE);
    EXPECT_TRUE(rom.year() && *(rom.year()) == ROM_YEAR);
    EXPECT_TRUE(rom.manufacturer() && *(rom.manufacturer()) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom.isBios() && *(rom.isBios()) == ROM_IS_BIOS);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Rom from a json which only contains the rom path.
        Expectation: Rom is correctly built but contains no additional information except from path.
    */
    rom = nlohmann::json{{Rom::PATH_JSON_FIELD, ROM_PATH}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_FALSE(rom.title());
    EXPECT_FALSE(rom.year());
    EXPECT_FALSE(rom.manufacturer());
    EXPECT_FALSE(rom.isBios());
    EXPECT_FALSE(rom.screenshot());

    /*
        Building Rom from a json which does not contain a path.
        Expectation: Construction will throw since this should not be allowed.
    */
    EXPECT_THROW(
        (Rom{nlohmann::json{{Rom::INFO_JSON_FIELD, INFO_SET_COMPLETE}, {Rom::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}}}),
        Rom::Excep);

    /*
        Building Rom from a json which contains all the information except for the rom title.
        Expectation: Rom is correctly built and every field is correctly set apart from the rom title.
    */
    rom = nlohmann::json{{Rom::PATH_JSON_FIELD, ROM_PATH},
                         {Rom::INFO_JSON_FIELD, INFO_SET_TITLE_MISSING},
                         {Rom::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_FALSE(rom.title());
    EXPECT_TRUE(rom.year() && *(rom.year()) == ROM_YEAR);
    EXPECT_TRUE(rom.manufacturer() && *(rom.manufacturer()) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom.isBios() && *(rom.isBios()) == ROM_IS_BIOS);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Rom from a json which contains all the information except for the rom year.
        Expectation: Rom is correctly built and every field is correctly set apart from the rom year.
    */
    rom = nlohmann::json{{Rom::PATH_JSON_FIELD, ROM_PATH},
                         {Rom::INFO_JSON_FIELD, INFO_SET_YEAR_MISSING},
                         {Rom::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.title() && *(rom.title()) == ROM_TITLE);
    EXPECT_FALSE(rom.year());
    EXPECT_TRUE(rom.manufacturer() && *(rom.manufacturer()) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom.isBios() && *(rom.isBios()) == ROM_IS_BIOS);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Rom from a json which contains all the information except for the rom manufacturer.
        Expectation: Rom is correctly built and every field is correctly set apart from the rom manufacturer.
    */
    rom = nlohmann::json{{Rom::PATH_JSON_FIELD, ROM_PATH},
                         {Rom::INFO_JSON_FIELD, INFO_SET_MANUFACTURER_MISSING},
                         {Rom::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.title() && *(rom.title()) == ROM_TITLE);
    EXPECT_TRUE(rom.year() && *(rom.year()) == ROM_YEAR);
    EXPECT_FALSE(rom.manufacturer());
    EXPECT_TRUE(rom.isBios() && *(rom.isBios()) == ROM_IS_BIOS);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Rom from a json which contains all the information except for the rom bios.
        Expectation: Rom is correctly built and every field is correctly set apart from the rom bios.
    */
    rom = nlohmann::json{{Rom::PATH_JSON_FIELD, ROM_PATH},
                         {Rom::INFO_JSON_FIELD, INFO_SET_BIOS_MISSING},
                         {Rom::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.title() && *(rom.title()) == ROM_TITLE);
    EXPECT_TRUE(rom.year() && *(rom.year()) == ROM_YEAR);
    EXPECT_TRUE(rom.manufacturer() && *(rom.manufacturer()) == ROM_MANUFACTURER);
    EXPECT_FALSE(rom.isBios());
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Rom from a json which contains no information.
        Expectation: Rom is correctly built and no information is set.
    */
    rom = nlohmann::json{{Rom::PATH_JSON_FIELD, ROM_PATH},
                         {Rom::INFO_JSON_FIELD, INFO_SET_EMPTY},
                         {Rom::MEDIA_JSON_FIELD, MEDIA_SET_COMPLETE}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_FALSE(rom.title());
    EXPECT_FALSE(rom.year());
    EXPECT_FALSE(rom.manufacturer());
    EXPECT_FALSE(rom.isBios());
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Rom from a json which contains all the information except for the screenshot.
        Expectation: Rom is correctly built and every field is correctly set apart from the screenshot.
    */
    rom = nlohmann::json{{Rom::PATH_JSON_FIELD, ROM_PATH},
                         {Rom::INFO_JSON_FIELD, INFO_SET_COMPLETE},
                         {Rom::MEDIA_JSON_FIELD, MEDIA_SET_SCREENSHOT_MISSING}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.title() && *(rom.title()) == ROM_TITLE);
    EXPECT_TRUE(rom.year() && *(rom.year()) == ROM_YEAR);
    EXPECT_TRUE(rom.manufacturer() && *(rom.manufacturer()) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom.isBios() && *(rom.isBios()) == ROM_IS_BIOS);
    EXPECT_FALSE(rom.screenshot());

    /*
        Building Rom from a json which contains no media information.
        Expectation: Rom is correctly built and no media is set.
    */
    rom = nlohmann::json{{Rom::PATH_JSON_FIELD, ROM_PATH},
                         {Rom::INFO_JSON_FIELD, INFO_SET_COMPLETE},
                         {Rom::MEDIA_JSON_FIELD, MEDIA_SET_EMPTY}};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.title() && *(rom.title()) == ROM_TITLE);
    EXPECT_TRUE(rom.year() && *(rom.year()) == ROM_YEAR);
    EXPECT_TRUE(rom.manufacturer() && *(rom.manufacturer()) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom.isBios() && *(rom.isBios()) == ROM_IS_BIOS);
    EXPECT_FALSE(rom.screenshot());
}

TEST(Rom, toJson)
{
    /*
        Building a JSON from a rom that contains all the information.
        Expectation: json contains all the information.
    */
    nlohmann::json json = Rom{ROM_PATH, INFO_SET_COMPLETE, MEDIA_SET_COMPLETE};

    EXPECT_TRUE(json.contains(Rom::PATH_JSON_FIELD) && json[Rom::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::TITLE_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::YEAR_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD].contains(RomInfo::MANUFACTURER_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD].contains(RomInfo::ISBIOS_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
    EXPECT_TRUE(json.contains(Rom::MEDIA_JSON_FIELD) &&
                json[Rom::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[Rom::MEDIA_JSON_FIELD][RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building a JSON from a rom that only contains a path.
        Expectation: json only contains info about the path.
    */
    json = Rom{ROM_PATH};

    EXPECT_TRUE(json.contains(Rom::PATH_JSON_FIELD) && json[Rom::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_FALSE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::TITLE_JSON_FIELD));
    EXPECT_FALSE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::YEAR_JSON_FIELD));
    EXPECT_FALSE(json.contains(Rom::INFO_JSON_FIELD) &&
                 json[Rom::INFO_JSON_FIELD].contains(RomInfo::MANUFACTURER_JSON_FIELD));
    EXPECT_FALSE(json.contains(Rom::INFO_JSON_FIELD) &&
                 json[Rom::INFO_JSON_FIELD].contains(RomInfo::ISBIOS_JSON_FIELD));
    EXPECT_FALSE(json.contains(Rom::MEDIA_JSON_FIELD) &&
                 json[Rom::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD));

    /*
        Building a JSON from a rom that contains all the information except for the rom title.
        Expectation: json contains all the information except for the rom title.
    */
    json = Rom{ROM_PATH, INFO_SET_TITLE_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_TRUE(json.contains(Rom::PATH_JSON_FIELD) && json[Rom::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_FALSE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::TITLE_JSON_FIELD));
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::YEAR_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD].contains(RomInfo::MANUFACTURER_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD].contains(RomInfo::ISBIOS_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
    EXPECT_TRUE(json.contains(Rom::MEDIA_JSON_FIELD) &&
                json[Rom::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[Rom::MEDIA_JSON_FIELD][RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building a JSON from a rom that contains all the information except the rom year.
        Expectation: json contains all the information except for the rom year.
    */
    json = Rom{ROM_PATH, INFO_SET_YEAR_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_TRUE(json.contains(Rom::PATH_JSON_FIELD) && json[Rom::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::TITLE_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_FALSE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::YEAR_JSON_FIELD));
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD].contains(RomInfo::MANUFACTURER_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD].contains(RomInfo::ISBIOS_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
    EXPECT_TRUE(json.contains(Rom::MEDIA_JSON_FIELD) &&
                json[Rom::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[Rom::MEDIA_JSON_FIELD][RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building a JSON from a rom that contains all the information except the rom manufacturer.
        Expectation: json contains all the information except for the rom manufacturer.
    */
    json = Rom{ROM_PATH, INFO_SET_MANUFACTURER_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_TRUE(json.contains(Rom::PATH_JSON_FIELD) && json[Rom::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::TITLE_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::YEAR_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_FALSE(json.contains(Rom::INFO_JSON_FIELD) &&
                 json[Rom::INFO_JSON_FIELD].contains(RomInfo::MANUFACTURER_JSON_FIELD));
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD].contains(RomInfo::ISBIOS_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
    EXPECT_TRUE(json.contains(Rom::MEDIA_JSON_FIELD) &&
                json[Rom::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[Rom::MEDIA_JSON_FIELD][RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building a JSON from a rom that contains all the information except for the bios.
        Expectation: json contains all the information except for the bios.
    */
    json = Rom{ROM_PATH, INFO_SET_BIOS_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_TRUE(json.contains(Rom::PATH_JSON_FIELD) && json[Rom::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::TITLE_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::YEAR_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD].contains(RomInfo::MANUFACTURER_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_FALSE(json.contains(Rom::INFO_JSON_FIELD) &&
                 json[Rom::INFO_JSON_FIELD].contains(RomInfo::ISBIOS_JSON_FIELD));
    EXPECT_TRUE(json.contains(Rom::MEDIA_JSON_FIELD) &&
                json[Rom::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[Rom::MEDIA_JSON_FIELD][RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building a JSON from a rom that contains no rom info.
        Expectation: json contains no rom info.
    */
    json = Rom{ROM_PATH, INFO_SET_EMPTY, MEDIA_SET_COMPLETE};

    EXPECT_TRUE(json.contains(Rom::PATH_JSON_FIELD) && json[Rom::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_FALSE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::TITLE_JSON_FIELD));
    EXPECT_FALSE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::YEAR_JSON_FIELD));
    EXPECT_FALSE(json.contains(Rom::INFO_JSON_FIELD) &&
                 json[Rom::INFO_JSON_FIELD].contains(RomInfo::MANUFACTURER_JSON_FIELD));
    EXPECT_FALSE(json.contains(Rom::INFO_JSON_FIELD) &&
                 json[Rom::INFO_JSON_FIELD].contains(RomInfo::ISBIOS_JSON_FIELD));
    EXPECT_TRUE(json.contains(Rom::MEDIA_JSON_FIELD) &&
                json[Rom::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[Rom::MEDIA_JSON_FIELD][RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building a JSON from a rom that contains all the information except the screenshot.
        Expectation: json contains all the information except for the screenshot.
    */
    json = Rom{ROM_PATH, INFO_SET_COMPLETE, MEDIA_SET_SCREENSHOT_MISSING};

    EXPECT_TRUE(json.contains(Rom::PATH_JSON_FIELD) && json[Rom::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::TITLE_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::YEAR_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD].contains(RomInfo::MANUFACTURER_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD].contains(RomInfo::ISBIOS_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
    EXPECT_FALSE(json.contains(Rom::MEDIA_JSON_FIELD) &&
                 json[Rom::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD));

    /*
        Building a JSON from a rom that contains no rom media.
        Expectation: json contains no rom media.
    */
    json = Rom{ROM_PATH, INFO_SET_COMPLETE, MEDIA_SET_EMPTY};

    EXPECT_TRUE(json.contains(Rom::PATH_JSON_FIELD) && json[Rom::PATH_JSON_FIELD] == ROM_PATH.string());
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::TITLE_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) && json[Rom::INFO_JSON_FIELD].contains(RomInfo::YEAR_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD].contains(RomInfo::MANUFACTURER_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(Rom::INFO_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD].contains(RomInfo::ISBIOS_JSON_FIELD) &&
                json[Rom::INFO_JSON_FIELD][RomInfo::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
    EXPECT_FALSE(json.contains(Rom::MEDIA_JSON_FIELD) &&
                 json[Rom::MEDIA_JSON_FIELD].contains(RomMedia::SCREENSHOT_JSON_FIELD));
}

TEST(Rom, fromConstructor)
{
    /*
        Building Rom from a constructor which contains all the information.
        Expectation: Rom is correctly built and every field is correctly set.
    */
    Rom rom{ROM_PATH, INFO_SET_COMPLETE, MEDIA_SET_COMPLETE};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.title() && *(rom.title()) == ROM_TITLE);
    EXPECT_TRUE(rom.year() && *(rom.year()) == ROM_YEAR);
    EXPECT_TRUE(rom.manufacturer() && *(rom.manufacturer()) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom.isBios() && *(rom.isBios()) == ROM_IS_BIOS);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Rom from a constructor which only contains the rom path.
        Expectation: Rom is correctly built but contains no additional information except from path.
    */
    rom = Rom{ROM_PATH};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_FALSE(rom.title());
    EXPECT_FALSE(rom.year());
    EXPECT_FALSE(rom.manufacturer());
    EXPECT_FALSE(rom.isBios());
    EXPECT_FALSE(rom.screenshot());

    /*
        Building Rom from a constructor which contains all the information except for the rom title.
        Expectation: Rom is correctly built and every field is correctly set apart from the rom title.
    */
    rom = Rom{ROM_PATH, INFO_SET_TITLE_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_FALSE(rom.title());
    EXPECT_TRUE(rom.year() && *(rom.year()) == ROM_YEAR);
    EXPECT_TRUE(rom.manufacturer() && *(rom.manufacturer()) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom.isBios() && *(rom.isBios()) == ROM_IS_BIOS);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Rom from a constructor which contains all the information except for the rom year.
        Expectation: Rom is correctly built and every field is correctly set apart from the rom year.
    */
    rom = Rom{ROM_PATH, INFO_SET_YEAR_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.title() && *(rom.title()) == ROM_TITLE);
    EXPECT_FALSE(rom.year());
    EXPECT_TRUE(rom.manufacturer() && *(rom.manufacturer()) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom.isBios() && *(rom.isBios()) == ROM_IS_BIOS);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Rom from a json constructor contains all the information except for the rom manufacturer.
        Expectation: Rom is correctly built and every field is correctly set apart from the rom manufacturer.
    */
    rom = Rom{ROM_PATH, INFO_SET_MANUFACTURER_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.title() && *(rom.title()) == ROM_TITLE);
    EXPECT_TRUE(rom.year() && *(rom.year()) == ROM_YEAR);
    EXPECT_FALSE(rom.manufacturer());
    EXPECT_TRUE(rom.isBios() && *(rom.isBios()) == ROM_IS_BIOS);
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Rom from a constructor which contains all the information except for the rom bios.
        Expectation: Rom is correctly built and every field is correctly set apart from the rom bios.
    */
    rom = Rom{ROM_PATH, INFO_SET_BIOS_MISSING, MEDIA_SET_COMPLETE};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.title() && *(rom.title()) == ROM_TITLE);
    EXPECT_TRUE(rom.year() && *(rom.year()) == ROM_YEAR);
    EXPECT_TRUE(rom.manufacturer() && *(rom.manufacturer()) == ROM_MANUFACTURER);
    EXPECT_FALSE(rom.isBios());
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Rom from a constructor which contains no information.
        Expectation: Rom is correctly built and no information is set.
    */
    rom = Rom{ROM_PATH, INFO_SET_EMPTY, MEDIA_SET_COMPLETE};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_FALSE(rom.title());
    EXPECT_FALSE(rom.year());
    EXPECT_FALSE(rom.manufacturer());
    EXPECT_FALSE(rom.isBios());
    EXPECT_TRUE(rom.screenshot() && *(rom.screenshot()) == SCREENSHOT_PATH);

    /*
        Building Rom from a constructor which contains all the information except for the screenshot.
        Expectation: Rom is correctly built and every field is correctly set apart from the screenshot.
    */
    rom = Rom{ROM_PATH, INFO_SET_COMPLETE, MEDIA_SET_SCREENSHOT_MISSING};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.title() && *(rom.title()) == ROM_TITLE);
    EXPECT_TRUE(rom.year() && *(rom.year()) == ROM_YEAR);
    EXPECT_TRUE(rom.manufacturer() && *(rom.manufacturer()) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom.isBios() && *(rom.isBios()) == ROM_IS_BIOS);
    EXPECT_FALSE(rom.screenshot());

    /*
        Building Rom from a constructor which contains no media information.
        Expectation: Rom is correctly built and no media is set.
    */
    rom = Rom{ROM_PATH, INFO_SET_COMPLETE, MEDIA_SET_EMPTY};

    EXPECT_EQ(rom.path(), ROM_PATH);
    EXPECT_TRUE(rom.title() && *(rom.title()) == ROM_TITLE);
    EXPECT_TRUE(rom.year() && *(rom.year()) == ROM_YEAR);
    EXPECT_TRUE(rom.manufacturer() && *(rom.manufacturer()) == ROM_MANUFACTURER);
    EXPECT_TRUE(rom.isBios() && *(rom.isBios()) == ROM_IS_BIOS);
    EXPECT_FALSE(rom.screenshot());
}
