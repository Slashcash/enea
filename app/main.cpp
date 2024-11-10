#include <exception>
#include <fmt/core.h>
#include <magic_enum.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include "configuration.hpp"
#include "emulator.hpp"
#include "gui.hpp"
#include "rom.hpp"
#include "romsource.hpp"
#include "softwareinfo.hpp"

int main()
{
    try
    {
        // Loading log level from environment variable
        spdlog::cfg::load_env_levels();

        spdlog::info("Starting {} {}", projectName, projectVersion);

        // Searching for advmame
        spdlog::info("Searching for advanceMAME on the system");
        Emulator emulator;
        if (auto emulatorInfo = emulator.info(); !emulatorInfo)
        {
            spdlog::error("advanceMAME not found on the system");
            return 1;
        }

        // Creating useful folders
        auto romPath = Configuration::get().romDirectory();

        std::error_code createFolderEc;
        std::filesystem::create_directories(romPath, createFolderEc);
        if (createFolderEc)
        {
            spdlog::warn("Rom folder creation failed at {}", romPath.string());
        }

        auto cachePath = Configuration::get().cacheDirectory();
        std::filesystem::create_directories(cachePath, createFolderEc);
        if (createFolderEc)
        {
            spdlog::warn("Cache folder creation failed at {}", cachePath.string());
        }

        // Searching for roms
        spdlog::info("Searching for roms and media");
        RomSource romSource(romPath, cachePath);
        std::vector<Rom> romList = romSource.scan();

        spdlog::info("Saving cache information");
        if (!romSource.saveOnCache())
        {
            spdlog::warn("Failed to write cache file, cache will not be available");
        }

        // Starting gui
        Gui gui(romList);
        gui.run();

        spdlog::info("Stopping {} {}", projectName, projectVersion);
        return 0;
    }
    catch (const enea::Exception& excep)
    {
        spdlog::error(excep.what());
    }
}
