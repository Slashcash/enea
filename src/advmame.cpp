#include "advmame.hpp"

#include <xmldatabase.hpp>

AdvMAME::AdvMAME(const fs::path& theEmulatorPath) : Emulator(theEmulatorPath, "AdvMAME", "0.106") {
    configuration_file_path = theEmulatorPath.parent_path() / "options.cfg";
    emulator_database = std::make_shared<XMLDataBase>();
}

Result AdvMAME::runEffectively(const fs::path& theRomPath) const {
    //we build the command
    std::string console_command = getPath().string() + " -dir_rom " + theRomPath.parent_path().string() + " -cfg " + configuration_file_path.string() + " " + theRomPath.stem().string();
    unsigned int console_value = system(console_command.c_str());

    return Result(console_value);
}
