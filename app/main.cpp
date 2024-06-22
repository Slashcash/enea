#include <exception>
#include <fmt/core.h>
#include <magic_enum.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include "cache.hpp"
#include "configuration.hpp"
#include "emulator.hpp"
#include "gui.hpp"
#include "rom.hpp"
#include "romdb.hpp"
#include "romsource.hpp"
#include "softwareinfo.hpp"

int main()
{
    try
    {
        // Loading log level from environment variable
        spdlog::cfg::load_env_levels();

        spdlog::info("Starting {} v{}", projectName, projectVersion);

        // Searching for advmame
        spdlog::info("Searching for advanceMAME on the system");
        Emulator emulator;
        if (auto emulatorInfo = emulator.info(); !emulatorInfo)
        {
            spdlog::error("advanceMAME not found on the system");
            return 1;
        }

        // Creating useful folders
        Configuration config;
        auto romPath = config.romDirectory();

        std::error_code createFolderEc;
        std::filesystem::create_directories(romPath, createFolderEc);
        if (createFolderEc)
        {
            spdlog::warn("Rom folder creation failed at {}", romPath.string());
        }

        auto cacheFile = config.cacheFile();
        auto cachePath = cacheFile.parent_path();
        std::filesystem::create_directories(cachePath, createFolderEc);
        if (createFolderEc)
        {
            spdlog::warn("Cache folder creation failed at {}", cachePath.string());
        }

        // Searching for roms
        spdlog::info("Searching for roms");
        std::list<Rom> romList;
        RomSource romSource(romPath);
        auto lastEditTime = romSource.lastEditTime();
        spdlog::debug("Trying to load cache file from: {}", cachePath.string());
        Cache cache(config.cacheFile());

        if (auto error = cache.load(); error || lastEditTime != *(cache.lastEditTime()))
        {
            // Scanning folder
            spdlog::debug("Scanning for roms in {}", romPath.string());
            auto scanResult = romSource.scan();

            // No rom found: we throw
            if (scanResult.roms.empty())
            {
                throw Exception("No rom found");
            }

            // Retrieving rom information
            RomDB romdb;
            spdlog::debug("Loading rom database");
            romdb.load();
            for (const auto& file : scanResult.roms)
            {
                // Searching rom information into the database
                RomInfo romInfo;
                spdlog::trace("Querying rom database for: {}", file.string());
                // Adding it to the database if: query is succesful && rom is not a bios
                if (auto query = romdb.find(file); query && (!query->isBios || !(*(query->isBios))))
                {
                    // Searching if we found any media associated to the rom in the rom folder
                    RomMedia media;
                    for (const auto& screenshot : scanResult.screenshots)
                    {
                        if (screenshot.filename().string().starts_with(file.stem().string()))
                        {
                            media = RomMedia{.screenshot = screenshot};
                        }
                    }

                    romList.emplace_back(file, *query, media);
                }
                else
                {
                    spdlog::warn("No information in rom database for: {}, not adding it to the rom list",
                                 file.stem().string());
                }
            }

            cache.setRomCache(romSource.romFolder(), romList, scanResult.lastModified);
        }

        else
        {
            romList = *(cache.roms());
        }

        // Printing a convenient list of added roms
        for (const auto& rom : romList)
        {
            spdlog::trace("Found rom: {}", rom.path().string());
        }

        spdlog::info("Saving information");
        spdlog::debug("Writing cache file at: {}", cachePath.string());
        if (auto error = cache.write(); error.has_value())
        {
            spdlog::warn("Failed to write cache file at: {}, cache will not be available", cachePath.string());
        }

        // Starting gui
        Gui gui(romList);
        gui.run();

        spdlog::info("Stopping {} v{}", projectName, projectVersion);
        return 0;
    }
    catch (const Exception& excep)
    {
        spdlog::error("{}", excep.what());
    }
}
