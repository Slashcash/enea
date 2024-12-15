from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMake

class Recipe(ConanFile):
    generators = "CMakeDeps", "VirtualBuildEnv", "VirtualRunEnv"
    settings = "build_type", "arch", "os"
    exports_sources = "*"

    def generate(self):
        tc = CMakeToolchain(self)

        if self.settings.os == "Linux" and (self.settings.arch == "armv7hf" or self.settings.arch == "armv8"):
            tc.variables["USE_POSIX_FILE_LIST"] = True
        else:
            tc.variables["USE_POSIX_FILE_LIST"] = False

        tc.generate()

    def configure(self):
        self.options["sfml"].network = False

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if not self.conf.get("tools.build:skip_test", default=False):
            cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def requirements(self):
        self.requires("flac/1.4.3", override=True)
        self.requires("spdlog/1.12.0")
        self.requires("magic_enum/0.9.3")
        self.requires("nlohmann_json/3.11.2")
        self.requires("sfml/2.6.0")
        self.requires("rocket/cci.20200603")
        self.requires("stduuid/1.2.3")
        self.requires("chef-fun/cci.20233110")
        # We do not try to compile advmame on Windows for now
        if self.settings.os == "Linux":
            self.requires("advmame/4.0")

    def build_requirements(self):
        if not self.conf.get("tools.build:skip_test", default=False):
            self.test_requires("gtest/1.14.0")

    def layout(self):
        cmake_layout(self)
