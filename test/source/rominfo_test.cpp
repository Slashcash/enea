#include "rominfo.hpp"

#include <gtest/gtest.h>

static const std::string ROM_TITLE = "Street Fighter II: The World Warrior";
static const std::string ROM_YEAR = "1991";
static const std::string ROM_MANUFACTURER = "Capcom";
static constexpr bool ROM_IS_BIOS = false;

TEST(RomInfo, fromJson)
{
    /*
        Building RomInfo from a json which has a complete set of information.
        Expectation: RomInfo struct has every field correctly set.
    */
    const nlohmann::json INFO_SET_COMPLETE{{RomInfo::TITLE_JSON_FIELD, ROM_TITLE},
                                           {RomInfo::YEAR_JSON_FIELD, ROM_YEAR},
                                           {RomInfo::MANUFACTURER_JSON_FIELD, ROM_MANUFACTURER},
                                           {RomInfo::ISBIOS_JSON_FIELD, ROM_IS_BIOS}};

    auto romInfo = INFO_SET_COMPLETE.template get<RomInfo>();

    EXPECT_TRUE(romInfo.title && *(romInfo.title) == ROM_TITLE);
    EXPECT_TRUE(romInfo.year && *(romInfo.year) == ROM_YEAR);
    EXPECT_TRUE(romInfo.manufacturer && *(romInfo.manufacturer) == ROM_MANUFACTURER);
    EXPECT_TRUE(romInfo.isBios && *(romInfo.isBios) == ROM_IS_BIOS);

    /*
        Building RomInfo from a json which has an incomplete set of information. It misses rom title.
        Expectation: RomInfo struct has every field correctly set apart for title which should be empty.
    */
    const nlohmann::json INFO_SET_TITLE_MISSING{{RomInfo::YEAR_JSON_FIELD, ROM_YEAR},
                                                {RomInfo::MANUFACTURER_JSON_FIELD, ROM_MANUFACTURER},
                                                {RomInfo::ISBIOS_JSON_FIELD, ROM_IS_BIOS}};

    romInfo = INFO_SET_TITLE_MISSING.template get<RomInfo>();

    EXPECT_FALSE(romInfo.title);
    EXPECT_TRUE(romInfo.year && *(romInfo.year) == ROM_YEAR);
    EXPECT_TRUE(romInfo.manufacturer && *(romInfo.manufacturer) == ROM_MANUFACTURER);
    EXPECT_TRUE(romInfo.isBios && *(romInfo.isBios) == ROM_IS_BIOS);

    /*
        Building RomInfo from a json which has an incomplete set of information. It misses rom year.
        Expectation: RomInfo struct has every field correctly set apart for year which should be empty.
    */
    const nlohmann::json INFO_SET_YEAR_MISSING{{RomInfo::TITLE_JSON_FIELD, ROM_TITLE},
                                               {RomInfo::MANUFACTURER_JSON_FIELD, ROM_MANUFACTURER},
                                               {RomInfo::ISBIOS_JSON_FIELD, ROM_IS_BIOS}};

    romInfo = INFO_SET_YEAR_MISSING.template get<RomInfo>();

    EXPECT_TRUE(romInfo.title && *(romInfo.title) == ROM_TITLE);
    EXPECT_FALSE(romInfo.year);
    EXPECT_TRUE(romInfo.manufacturer && *(romInfo.manufacturer) == ROM_MANUFACTURER);
    EXPECT_TRUE(romInfo.isBios && *(romInfo.isBios) == ROM_IS_BIOS);

    /*
        Building RomInfo from a json which has an incomplete set of information. It misses rom manufacturer.
        Expectation: RomInfo struct has every field correctly set apart for manufacturer which should be empty
    */
    const nlohmann::json INFO_SET_MANUFACTURER_MISSING{{RomInfo::TITLE_JSON_FIELD, ROM_TITLE},
                                                       {RomInfo::YEAR_JSON_FIELD, ROM_YEAR},
                                                       {RomInfo::ISBIOS_JSON_FIELD, ROM_IS_BIOS}};

    romInfo = INFO_SET_MANUFACTURER_MISSING.template get<RomInfo>();

    EXPECT_TRUE(romInfo.title && *(romInfo.title) == ROM_TITLE);
    EXPECT_TRUE(romInfo.year && *(romInfo.year) == ROM_YEAR);
    EXPECT_FALSE(romInfo.manufacturer);
    EXPECT_TRUE(romInfo.isBios && *(romInfo.isBios) == ROM_IS_BIOS);

    /*
        Building RomInfo from a json which has an incomplete set of information. It misses bios.
        Expectation: RomInfo struct has every field correctly set apart fo bios which should be empty.
    */

    const nlohmann::json INFO_SET_BIOS_MISSING{{RomInfo::TITLE_JSON_FIELD, ROM_TITLE},
                                               {RomInfo::YEAR_JSON_FIELD, ROM_YEAR},
                                               {RomInfo::MANUFACTURER_JSON_FIELD, ROM_MANUFACTURER}};

    romInfo = INFO_SET_BIOS_MISSING.template get<RomInfo>();

    EXPECT_TRUE(romInfo.title && *(romInfo.title) == ROM_TITLE);
    EXPECT_TRUE(romInfo.year && *(romInfo.year) == ROM_YEAR);
    EXPECT_TRUE(romInfo.manufacturer && *(romInfo.manufacturer) == ROM_MANUFACTURER);
    EXPECT_FALSE(romInfo.isBios);

    /*
        Building RomInfo from an invalid json (it contains random data).
        Expectation: RomInfo struct has no field set.
    */
    const nlohmann::json INFO_SET_INVALID{{"error", "ignored"}};

    romInfo = INFO_SET_INVALID.template get<RomInfo>();

    EXPECT_FALSE(romInfo.title);
    EXPECT_FALSE(romInfo.year);
    EXPECT_FALSE(romInfo.manufacturer);
    EXPECT_FALSE(romInfo.isBios);

    /*
        Building RomInfo from an empty json.
        Expectation: RomInfo struct has no field set.
    */
    const nlohmann::json INFO_SET_EMPTY;

    romInfo = INFO_SET_EMPTY.template get<RomInfo>();

    EXPECT_FALSE(romInfo.title);
    EXPECT_FALSE(romInfo.year);
    EXPECT_FALSE(romInfo.manufacturer);
    EXPECT_FALSE(romInfo.isBios);
}

TEST(RomInfo, toJson)
{
    /*
        Building JSON from RomInfo which has a complete set of information.
        Expectation: the JSON has every field correctly set.
     */
    nlohmann::json json =
        RomInfo{.title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};

    EXPECT_TRUE(json.contains(RomInfo::TITLE_JSON_FIELD) && json[RomInfo::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(RomInfo::YEAR_JSON_FIELD) && json[RomInfo::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(RomInfo::MANUFACTURER_JSON_FIELD) &&
                json[RomInfo::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(RomInfo::ISBIOS_JSON_FIELD) && json[RomInfo::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);

    /*
        Building JSON from RomInfo which has an incomplete set of information. It misses title.
        Expectation: the JSON has every field correctly set apart for title which should be empty.
    */
    json = RomInfo{.year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};

    EXPECT_FALSE(json.contains(RomInfo::TITLE_JSON_FIELD));
    EXPECT_TRUE(json.contains(RomInfo::YEAR_JSON_FIELD) && json[RomInfo::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(RomInfo::MANUFACTURER_JSON_FIELD) &&
                json[RomInfo::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(RomInfo::ISBIOS_JSON_FIELD) && json[RomInfo::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);

    /*
        Building JSON from RomInfo which has an incomplete set of information. It misses year.
        Expectation: the JSON has every field correctly set apart for year which should be empty.
    */
    json = RomInfo{.title = ROM_TITLE, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};

    EXPECT_TRUE(json.contains(RomInfo::TITLE_JSON_FIELD) && json[RomInfo::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_FALSE(json.contains(RomInfo::YEAR_JSON_FIELD));
    EXPECT_TRUE(json.contains(RomInfo::MANUFACTURER_JSON_FIELD) &&
                json[RomInfo::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(RomInfo::ISBIOS_JSON_FIELD) && json[RomInfo::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);

    /*
        Building JSON from RomInfo which has a complete set of information. It misses manufacturer.
        Expectation: the JSON has every field correctly set apart for manufacturer which should be empty.
    */
    json = RomInfo{.title = ROM_TITLE, .year = ROM_YEAR, .isBios = ROM_IS_BIOS};

    EXPECT_TRUE(json.contains(RomInfo::TITLE_JSON_FIELD) && json[RomInfo::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(RomInfo::YEAR_JSON_FIELD) && json[RomInfo::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_FALSE(json.contains(RomInfo::MANUFACTURER_JSON_FIELD));
    EXPECT_TRUE(json.contains(RomInfo::ISBIOS_JSON_FIELD) && json[RomInfo::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);

    /*
        Building JSON from RomInfo which has an incomplete set of information. It misses bios
        Expectation: the JSON has every field correctly set apart for bios which should be empty.
    */
    json = RomInfo{.title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER};

    EXPECT_TRUE(json.contains(RomInfo::TITLE_JSON_FIELD) && json[RomInfo::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(RomInfo::YEAR_JSON_FIELD) && json[RomInfo::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(RomInfo::MANUFACTURER_JSON_FIELD) &&
                json[RomInfo::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_FALSE(json.contains(RomInfo::ISBIOS_JSON_FIELD));

    /*
        Building JSON from empty RomInfo.
        Expectation: the JSON has no field set.
     */
    json = RomInfo{};

    EXPECT_FALSE(json.contains(RomInfo::TITLE_JSON_FIELD));
    EXPECT_FALSE(json.contains(RomInfo::YEAR_JSON_FIELD));
    EXPECT_FALSE(json.contains(RomInfo::MANUFACTURER_JSON_FIELD));
    EXPECT_FALSE(json.contains(RomInfo::ISBIOS_JSON_FIELD));
}
