from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMake

class Recipe(ConanFile):
    generators = "CMakeDeps", "VirtualBuildEnv", "VirtualRunEnv"
    settings = "build_type"
    options = {"build_tests": [True, False]}
    default_options = {"build_tests": True}
    exports_sources = "*"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["BUILD_TESTS"] = self.options.build_tests
        tc.generate()

    def configure(self):
        self.options["sfml"].network = False

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def requirements(self):
        self.requires("spdlog/1.12.0")
        self.requires("tinyxml2/9.0.0")
        self.requires("magic_enum/0.9.3")
        self.requires("nlohmann_json/3.11.2")
        self.requires("sfml/2.6.0")
        self.requires("rocket/2.0")

    def build_requirements(self):
        if self.options.build_tests:
            self.test_requires("gtest/1.14.0")

    def layout(self):
        cmake_layout(self)
