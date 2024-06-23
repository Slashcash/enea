from conan import ConanFile
from conan.tools.gnu import AutotoolsDeps, Autotools, AutotoolsToolchain, PkgConfigDeps
from conan.tools.scm import Git

required_conan_version = ">=2.0.0"

class AdvMameConan(ConanFile):
    name = "advmame"
    description = "A MAME and MESS port with an advanced video support for Arcade Monitors, TVs, and PC Monitors."
    license = "GNU"
    topics = "emulator"
    homepage = "http://www.advancemame.it/"
    package_type = "application"
    settings = "os", "arch", "compiler", "build_type"
    version = "3.10"
    generators = "AutotoolsToolchain", "PkgConfigDeps", "VirtualBuildEnv"

    def source(self):
        git = Git(self)
        git.clone(url="https://github.com/amadvance/advancemame.git", target=".")
        git.checkout("821679cb29201c77cd28c2b60b77778d54bdcc88")

    def requirements(self):
        self.requires("sdl/2.28.5")

    def build_requirements(self):
        self.tool_requires("pkgconf/2.1.0")
        self.tool_requires("automake/1.16.5")
        self.tool_requires("autoconf/2.71")

    def build(self):
        autotools = Autotools(self)
        autotools.autoreconf()
        autotools.configure(args=["--disable-fb"])
        autotools.make()

    def package(self):
        autotools = Autotools(self)
        autotools.install()
