from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMake
from conan.errors import ConanInvalidConfiguration
from conan.tools.files import load
import re, os


class EneaRecipe(ConanFile):
    name = "enea"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    options = {"build_tests": [True, False]}
    default_options = {"build_tests": True}

    def set_version(self):
        content = load(self, os.path.join(self.recipe_folder, "cmake", "manageOptions.cmake"))
        version = re.search(r"set\(SOFTWARE_VERSION (.*)\)", content).group(1)
        self.version = version.strip()

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["ENEA_BUILD_TESTS"] = self.options.build_tests
        tc.generate()

    def configure(self):
        self.options["sfml"].network = False

    def requirements(self):
        self.requires("spdlog/1.12.0")
        self.requires("magic_enum/0.9.3")
        self.requires("sfml/2.6.0")
        self.requires("rocket/2.0")
        self.requires("nlohmann_json/3.11.2")
        self.requires("tinyxml2/9.0.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.27.0")
        if self.options.build_tests:
            self.test_requires("gtest/1.14.0")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def validate(self):
        if self.settings.os != "Linux":
            raise ConanInvalidConfiguration("Invalid OS as a build target: only Linux is supported")
        if self.settings.arch != "x86_64":
            raise ConanInvalidConfiguration("Invalid architecture as a build target: only x86_64 is supported")
        if self.settings.compiler != "gcc" or self.settings.compiler.version != "11.3":
            raise ConanInvalidConfiguration("Invalid compiler detected: only gcc 11.3.0 is supported")
