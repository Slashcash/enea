#include "rommedia.hpp"

#include <gtest/gtest.h>

static const std::filesystem::path SCREENSHOT_PATH = std::filesystem::absolute("sf2.png");

TEST(RomMedia, fromJson)
{
    /*
        Building RomMedia from a json which has a complete set of information.
        Expectation: RomMedia struct has every field correctly set.
    */
    const nlohmann::json INFO_SET_COMPLETE{{RomMedia::SCREENSHOT_JSON_FIELD, SCREENSHOT_PATH}};

    auto romMedia = INFO_SET_COMPLETE.template get<RomMedia>();

    EXPECT_TRUE(romMedia.screenshot && *(romMedia.screenshot) == SCREENSHOT_PATH);

    /*
        Building RomMedia from a json which has an incomplete set of information. It misses the screenshot.
        Expectation: RomMedia struct has every field correctly set apart for screenshot which should be empty.
    */
    const nlohmann::json INFO_SET_SCREENSHOT_MISSING{};

    romMedia = INFO_SET_SCREENSHOT_MISSING.template get<RomMedia>();

    EXPECT_FALSE(romMedia.screenshot);

    /*
        Building RomMedia from an invalid json (it contains random data).
        Expectation: RomMedia struct has no field set.
    */
    const nlohmann::json INFO_SET_INVALID{{"error", "ignored"}};

    romMedia = INFO_SET_INVALID.template get<RomMedia>();

    EXPECT_FALSE(romMedia.screenshot);

    /*
        Building RomMedia from an empty json.
        Expectation: RomInfo struct has no field set.
    */
    const nlohmann::json INFO_SET_EMPTY;

    romMedia = INFO_SET_EMPTY.template get<RomMedia>();

    EXPECT_FALSE(romMedia.screenshot);
}

TEST(RomMedia, toJson)
{
    /*
        Building JSON from RomMedia which has a complete set of information.
        Expectation: the JSON has every field correctly set.
     */
    nlohmann::json json = RomMedia{
        .screenshot = SCREENSHOT_PATH,
    };

    EXPECT_TRUE(json.contains(RomMedia::SCREENSHOT_JSON_FIELD) &&
                json[RomMedia::SCREENSHOT_JSON_FIELD] == SCREENSHOT_PATH.string());

    /*
        Building JSON from RomMedia which has an incomplete set of information. It misses screenshot.
        Expectation: the JSON has every field correctly set apart for screenshot which should be empty.
    */
    json = RomMedia{};

    EXPECT_FALSE(json.contains(RomMedia::SCREENSHOT_JSON_FIELD));

    /*
        Building JSON from empty RomMedia.
        Expectation: the JSON has no field set.
     */
    json = RomMedia{};

    EXPECT_FALSE(json.contains(RomMedia::SCREENSHOT_JSON_FIELD));
}
