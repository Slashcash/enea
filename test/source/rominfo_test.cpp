#include "rom/info.hpp"

#include <gtest/gtest.h>

static const std::string ROM_TITLE = "Street Fighter II: The World Warrior";
static const std::string ROM_YEAR = "1991";
static const std::string ROM_MANUFACTURER = "Capcom";
static constexpr bool ROM_IS_BIOS = false;

/*
    Building RomInfo from a json which has a complete set of information.
    Expectation: RomInfo struct has every field correctly set.
*/
TEST(RomInfo, fromCompleteJson)
{
    auto romInfo = nlohmann::json{
        {Rom::Info::TITLE_JSON_FIELD, ROM_TITLE},
        {Rom::Info::YEAR_JSON_FIELD, ROM_YEAR},
        {Rom::Info::MANUFACTURER_JSON_FIELD, ROM_MANUFACTURER},
        {Rom::Info::ISBIOS_JSON_FIELD,
         ROM_IS_BIOS}}.template get<Rom::Info>();

    EXPECT_TRUE(romInfo.title == ROM_TITLE);
    EXPECT_TRUE(romInfo.year && *(romInfo.year) == ROM_YEAR);
    EXPECT_TRUE(romInfo.manufacturer && *(romInfo.manufacturer) == ROM_MANUFACTURER);
    EXPECT_TRUE(romInfo.isBios && *(romInfo.isBios) == ROM_IS_BIOS);
}

/*
    Building RomInfo from a json which has a missing title.
    Expectation: We throw as title is mandatory information.
*/
TEST(RomInfo, fromJsonMissingTitle)
{
    EXPECT_THROW((Rom::Info{nlohmann::json{{Rom::Info::YEAR_JSON_FIELD, ROM_YEAR},
                                           {Rom::Info::MANUFACTURER_JSON_FIELD, ROM_MANUFACTURER},
                                           {Rom::Info::ISBIOS_JSON_FIELD, ROM_IS_BIOS}}}),
                 nlohmann::json::exception);
}

/*
    Building RomInfo from a json which has an invalid title.
    Expectation: We throw as title is mandatory information.
*/
TEST(RomInfo, fromJsonInvalidTitle)
{
    EXPECT_THROW((Rom::Info{nlohmann::json{{Rom::Info::TITLE_JSON_FIELD, 0},
                                           {Rom::Info::YEAR_JSON_FIELD, ROM_YEAR},
                                           {Rom::Info::MANUFACTURER_JSON_FIELD, ROM_MANUFACTURER},
                                           {Rom::Info::ISBIOS_JSON_FIELD, ROM_IS_BIOS}}}),
                 nlohmann::json::exception);
}

/*
    Building RomInfo from a json which has an incomplete set of information. It misses rom year.
    Expectation: RomInfo struct has every field correctly set apart for year which should be empty.
*/
TEST(RomInfo, fromJsonMissingYear)
{
    auto romInfo = nlohmann::json{
        {Rom::Info::TITLE_JSON_FIELD, ROM_TITLE},
        {Rom::Info::MANUFACTURER_JSON_FIELD, ROM_MANUFACTURER},
        {Rom::Info::ISBIOS_JSON_FIELD,
         ROM_IS_BIOS}}.template get<Rom::Info>();

    EXPECT_TRUE(romInfo.title == ROM_TITLE);
    EXPECT_FALSE(romInfo.year);
    EXPECT_TRUE(romInfo.manufacturer && *(romInfo.manufacturer) == ROM_MANUFACTURER);
    EXPECT_TRUE(romInfo.isBios && *(romInfo.isBios) == ROM_IS_BIOS);
}

/*
    Building RomInfo from a json which has an invalid year.
    Expectation: RomInfo struct has every field correctly set apart for year which should be empty.
*/
TEST(RomInfo, fromJsonInvalidYear)
{
    auto romInfo = nlohmann::json{
        {Rom::Info::TITLE_JSON_FIELD, ROM_TITLE},
        {Rom::Info::YEAR_JSON_FIELD, 0},
        {Rom::Info::MANUFACTURER_JSON_FIELD, ROM_MANUFACTURER},
        {Rom::Info::ISBIOS_JSON_FIELD,
         ROM_IS_BIOS}}.template get<Rom::Info>();

    EXPECT_TRUE(romInfo.title == ROM_TITLE);
    EXPECT_FALSE(romInfo.year);
    EXPECT_TRUE(romInfo.manufacturer && *(romInfo.manufacturer) == ROM_MANUFACTURER);
    EXPECT_TRUE(romInfo.isBios && *(romInfo.isBios) == ROM_IS_BIOS);
}

/*
    Building RomInfo from a json which has an incomplete set of information. It misses rom manufacturer.
    Expectation: RomInfo struct has every field correctly set apart for manufacturer which should be empty
*/
TEST(RomInfo, fromJsonMissingManufacturer)
{
    auto romInfo = nlohmann::json{
        {Rom::Info::TITLE_JSON_FIELD, ROM_TITLE},
        {Rom::Info::YEAR_JSON_FIELD, ROM_YEAR},
        {Rom::Info::ISBIOS_JSON_FIELD,
         ROM_IS_BIOS}}.template get<Rom::Info>();

    EXPECT_TRUE(romInfo.title == ROM_TITLE);
    EXPECT_TRUE(romInfo.year && *(romInfo.year) == ROM_YEAR);
    EXPECT_FALSE(romInfo.manufacturer);
    EXPECT_TRUE(romInfo.isBios && *(romInfo.isBios) == ROM_IS_BIOS);
}

/*
    Building RomInfo from a json which has an invalid manufacturer.
    Expectation: RomInfo struct has every field correctly set apart for manufacturer which should be empty
*/
TEST(RomInfo, fromJsonInvalidManufacturer)
{

    auto romInfo = nlohmann::json{
        {Rom::Info::TITLE_JSON_FIELD, ROM_TITLE},
        {Rom::Info::MANUFACTURER_JSON_FIELD, 0},
        {Rom::Info::YEAR_JSON_FIELD, ROM_YEAR},
        {Rom::Info::ISBIOS_JSON_FIELD,
         ROM_IS_BIOS}}.template get<Rom::Info>();

    EXPECT_TRUE(romInfo.title == ROM_TITLE);
    EXPECT_TRUE(romInfo.year && *(romInfo.year) == ROM_YEAR);
    EXPECT_FALSE(romInfo.manufacturer);
    EXPECT_TRUE(romInfo.isBios && *(romInfo.isBios) == ROM_IS_BIOS);
}

/*
    Building RomInfo from a json which has an incomplete set of information. It misses bios.
    Expectation: RomInfo struct has every field correctly set apart fo bios which should be empty.
*/
TEST(RomInfo, fromJsonMissingBios)
{
    auto romInfo = nlohmann::json{{Rom::Info::TITLE_JSON_FIELD, ROM_TITLE},
                                  {Rom::Info::YEAR_JSON_FIELD, ROM_YEAR},
                                  {Rom::Info::MANUFACTURER_JSON_FIELD, ROM_MANUFACTURER}}
                       .template get<Rom::Info>();

    EXPECT_TRUE(romInfo.title == ROM_TITLE);
    EXPECT_TRUE(romInfo.year && *(romInfo.year) == ROM_YEAR);
    EXPECT_TRUE(romInfo.manufacturer && *(romInfo.manufacturer) == ROM_MANUFACTURER);
    EXPECT_FALSE(romInfo.isBios);
}

/*
    Building RomInfo from a json which has an incomplete bios.
    Expectation: RomInfo struct has every field correctly set apart fo bios which should be empty.
*/
TEST(RomInfo, fromJsonInvalidBios)
{
    auto romInfo = nlohmann::json{{Rom::Info::TITLE_JSON_FIELD, ROM_TITLE},
                                  {Rom::Info::YEAR_JSON_FIELD, ROM_YEAR},
                                  {Rom::Info::MANUFACTURER_JSON_FIELD, ROM_MANUFACTURER},
                                  {Rom::Info::ISBIOS_JSON_FIELD, "invalid"}}
                       .template get<Rom::Info>();

    EXPECT_TRUE(romInfo.title == ROM_TITLE);
    EXPECT_TRUE(romInfo.year && *(romInfo.year) == ROM_YEAR);
    EXPECT_TRUE(romInfo.manufacturer && *(romInfo.manufacturer) == ROM_MANUFACTURER);
    EXPECT_FALSE(romInfo.isBios);
}

/*
    Building JSON from RomInfo which has a complete set of information.
    Expectation: the JSON has every field correctly set.
 */
TEST(RomInfo, toJsonComplete)
{
    nlohmann::json json =
        Rom::Info{.title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};

    EXPECT_TRUE(json.contains(Rom::Info::TITLE_JSON_FIELD) && json[Rom::Info::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Rom::Info::YEAR_JSON_FIELD) && json[Rom::Info::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(Rom::Info::MANUFACTURER_JSON_FIELD) &&
                json[Rom::Info::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(Rom::Info::ISBIOS_JSON_FIELD) && json[Rom::Info::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
}

/*
    Building JSON from RomInfo which has an incomplete set of information. It misses year.
    Expectation: the JSON has every field correctly set apart for year which should be empty.
*/
TEST(RomInfo, toJsonMissingYear)
{
    nlohmann::json json = Rom::Info{.title = ROM_TITLE, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};

    EXPECT_TRUE(json.contains(Rom::Info::TITLE_JSON_FIELD) && json[Rom::Info::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_FALSE(json.contains(Rom::Info::YEAR_JSON_FIELD));
    EXPECT_TRUE(json.contains(Rom::Info::MANUFACTURER_JSON_FIELD) &&
                json[Rom::Info::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_TRUE(json.contains(Rom::Info::ISBIOS_JSON_FIELD) && json[Rom::Info::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
}

/*
    Building JSON from RomInfo which has a complete set of information. It misses manufacturer.
    Expectation: the JSON has every field correctly set apart for manufacturer which should be empty.
*/
TEST(RomInfo, toJsonMissingManufacturer)
{
    nlohmann::json json = Rom::Info{.title = ROM_TITLE, .year = ROM_YEAR, .isBios = ROM_IS_BIOS};

    EXPECT_TRUE(json.contains(Rom::Info::TITLE_JSON_FIELD) && json[Rom::Info::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Rom::Info::YEAR_JSON_FIELD) && json[Rom::Info::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_FALSE(json.contains(Rom::Info::MANUFACTURER_JSON_FIELD));
    EXPECT_TRUE(json.contains(Rom::Info::ISBIOS_JSON_FIELD) && json[Rom::Info::ISBIOS_JSON_FIELD] == ROM_IS_BIOS);
}

/*
    Building JSON from RomInfo which has an incomplete set of information. It misses bios.
    Expectation: the JSON has every field correctly set apart for bios which should be empty.
*/
TEST(RomInfo, toJsonMissingBios)
{
    nlohmann::json json = Rom::Info{.title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER};

    EXPECT_TRUE(json.contains(Rom::Info::TITLE_JSON_FIELD) && json[Rom::Info::TITLE_JSON_FIELD] == ROM_TITLE);
    EXPECT_TRUE(json.contains(Rom::Info::YEAR_JSON_FIELD) && json[Rom::Info::YEAR_JSON_FIELD] == ROM_YEAR);
    EXPECT_TRUE(json.contains(Rom::Info::MANUFACTURER_JSON_FIELD) &&
                json[Rom::Info::MANUFACTURER_JSON_FIELD] == ROM_MANUFACTURER);
    EXPECT_FALSE(json.contains(Rom::Info::ISBIOS_JSON_FIELD));
}

/*
    Converting RomInfo to a string.
    Expectation: it will return its title.
 */
TEST(RomInfo, toString)
{
    Rom::Info info{.title = ROM_TITLE, .year = ROM_YEAR, .manufacturer = ROM_MANUFACTURER, .isBios = ROM_IS_BIOS};
    EXPECT_EQ(info.toString(), ROM_TITLE);
}
