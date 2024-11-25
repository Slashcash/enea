#include "rom/media.hpp"

#include <gtest/gtest.h>

static const std::filesystem::path SCREENSHOT_PATH = std::filesystem::absolute("sf2.png");

/*
    Building RomMedia from a json which has a complete set of information.
    Expectation: RomMedia struct has every field correctly set.
*/
TEST(RomMedia, fromCompleteJson)
{
    auto romMedia = nlohmann::json{{Rom::Media::SCREENSHOT_JSON_FIELD, SCREENSHOT_PATH}}.template get<Rom::Media>();

    EXPECT_TRUE(romMedia.screenshot && *(romMedia.screenshot) == SCREENSHOT_PATH);
}

/*
    Building RomMedia from an empty json.
    Expectation: We throw.
*/
TEST(RomMedia, fromEmptyJson)
{
    EXPECT_THROW(nlohmann::json{}.template get<Rom::Media>(), nlohmann::json::exception);
}

/*
    Building RomMedia from a json which has an incomplete set of information. It misses the screenshot.
    Expectation: We throw.
*/
TEST(RomMedia, fromJsonMissingScreenshot)
{
    EXPECT_THROW(nlohmann::json{}.template get<Rom::Media>(), nlohmann::json::exception);
}

/*
    Building RomMedia from a json with an invalid screenshot path.
    Expectation: We throw.
*/
TEST(RomMedia, fromJsonInvalidScreenshot)
{
    EXPECT_THROW((nlohmann::json{{Rom::Media::SCREENSHOT_JSON_FIELD, 0}}.template get<Rom::Media>()),
                 nlohmann::json::exception);
}

/*
    Building JSON from RomMedia which has a complete set of information.
    Expectation: the JSON has every field correctly set.
 */
TEST(RomMedia, toJsonComplete)
{
    nlohmann::json json = Rom::Media{
        .screenshot = SCREENSHOT_PATH,
    };

    EXPECT_TRUE(json.contains(Rom::Media::SCREENSHOT_JSON_FIELD) &&
                json[Rom::Media::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());
}

/*
    Building JSON from RomMedia which has an incomplete set of information. It misses screenshot.
    Expectation: the JSON has every field correctly set apart for screenshot which should be empty.
*/
TEST(RomMedia, toJsonMissingScreenshot)
{
    nlohmann::json json = Rom::Media{};

    EXPECT_FALSE(json.contains(Rom::Media::SCREENSHOT_JSON_FIELD));
}

/*
    Building JSON from empty RomMedia.
    Expectation: the JSON has no field set.
 */
TEST(RomMedia, toJsonEmpty)
{
    nlohmann::json json = Rom::Media{};

    EXPECT_FALSE(json.contains(Rom::Media::SCREENSHOT_JSON_FIELD));
}
