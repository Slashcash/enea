#include <exception>
#include <fmt/core.h>
#include <magic_enum.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include "configuration.hpp"
#include "emulator.hpp"
#include "gui.hpp"
#include "rom/folder.hpp"
#include "rom/game.hpp"
#include "softwareinfo.hpp"

int main()
{
    try
    {
        // Loading log level from environment variable
        spdlog::cfg::load_env_levels();

        spdlog::info("Starting {} {}", projectName, projectVersion);

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
        Rom::Folder romFolder(romPath, cachePath);
        romFolder.monitor();
        std::vector<Rom::Game> romList = romFolder.elements();

        if (!romFolder.elements().empty())
        {
            std::ignore = romFolder.writeCache();
        }

        // If we found no roms we search for bundled roms as a fallback
        if (romList.empty())
        {
            Rom::Folder bundledRomFolder(Configuration::get().bundledRomDirectory(), cachePath);
            bundledRomFolder.monitor();
            romList = bundledRomFolder.elements();
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
