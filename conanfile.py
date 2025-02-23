from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMake

class Recipe(ConanFile):
    generators = "CMakeDeps", "VirtualBuildEnv", "VirtualRunEnv"
    settings = "build_type", "arch", "os"
    exports_sources = "*"
    sfml_options={}
    options = {
        "use_posix_file_list": [True, False],
        "use_direct_rendering": [True, False]
    }
    default_options = {
        "use_posix_file_list": False,
        "use_direct_rendering": False
    }

    def validate(self):
        if self.settings.os != "Linux":
            if self.options.use_posix_file_list:
                raise ConanInvalidConfiguration("Posix file list option is only available on posix systems")
            if self.options.use_direct_rendering:
                raise ConanInvalidConfiguration("Direct rendering is only available in Linux")

    def config_options(self):
        if self.settings.os == "Linux":
            if self.settings.arch == "armv7hf" or self.settings.arch == "armv8":
                self.options.use_posix_file_list = True

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["USE_POSIX_FILE_LIST"] = self.options.use_posix_file_list
        tc.variables["USE_DIRECT_RENDERING"] = self.options.use_direct_rendering

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

        if self.settings.os == "Linux":
            if self.options.use_direct_rendering:
                self.sfml_options={"use_drm": True}
        self.requires("sfml/2.6.2", options=self.sfml_options)

        self.requires("rocket/cci.20200603")
        self.requires("stduuid/1.2.3")
        self.requires("chef-fun/cci.20233110")
        self.requires("system2.cpp/cci.20241020")
        # We do not try to compile advmame on Windows for now
        if self.settings.os == "Linux":
            self.requires("advmame/4.0")

    def build_requirements(self):
        if not self.conf.get("tools.build:skip_test", default=False):
            self.test_requires("gtest/1.14.0")

    def layout(self):
        cmake_layout(self)
