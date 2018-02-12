#ifndef _ADVMAME_HPP_
#define _ADVMAME_HPP_

#include "emulator.hpp"

/*this class implements the AdvMAME support for this frontend*/

class AdvMAME : public Emulator {
    private:
        //members
        fs::path configuration_file_path;

        //functions
        virtual Result runEffectively(const fs::path& theRomPath) const;

    public:
        //ctor
        AdvMAME(const fs::path& theEmulatorPath);

        //getters
        fs::path getConfigurationFilePath() const { return configuration_file_path; }

        //setters
        void setConfigurationFilePath(const fs::path& theCFGFilePath) { configuration_file_path = theCFGFilePath; }
};

#endif // _ADVMAME_HPP_
