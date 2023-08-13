from conan import ConanFile
from conan.tools.files import get, replace_in_file
import shutil
import os

class Toolchain(ConanFile):
    name = "toolchain-linux-x86_64"
    version = "11.3.0"

    def source(self):
        get(self, "https://toolchains.bootlin.com/downloads/releases/toolchains/x86-64/tarballs/x86-64--glibc--stable-2022.08-1.tar.bz2",
                  strip_root=True)

    def package(self):
        shutil.copytree(os.path.join(self.build_folder, "bin"), os.path.join(self.package_folder, "bin"), dirs_exist_ok=True, symlinks=True)
        shutil.copytree(os.path.join(self.build_folder, "etc"), os.path.join(self.package_folder, "etc"), dirs_exist_ok=True, symlinks=True)
        shutil.copytree(os.path.join(self.build_folder, "include"), os.path.join(self.package_folder, "include"), dirs_exist_ok=True, symlinks=True)
        shutil.copytree(os.path.join(self.build_folder, "lib"), os.path.join(self.package_folder, "lib"), dirs_exist_ok=True, symlinks=True)
        shutil.copytree(os.path.join(self.build_folder, "libexec"), os.path.join(self.package_folder, "libexec"), dirs_exist_ok=True, symlinks=True)
        shutil.copytree(os.path.join(self.build_folder, "share"), os.path.join(self.package_folder, "share"), dirs_exist_ok=True, symlinks=True)
        shutil.copytree(os.path.join(self.build_folder, "x86_64-buildroot-linux-gnu"), os.path.join(self.package_folder, "x86_64-buildroot-linux-gnu"), dirs_exist_ok=True, symlinks=True)
        shutil.copy(os.path.join(self.build_folder, "buildroot.config"), self.package_folder, follow_symlinks=False)
        shutil.copy(os.path.join(self.build_folder, "lib64"), self.package_folder, follow_symlinks=False)
        shutil.copy(os.path.join(self.build_folder, "README.txt"), self.package_folder, follow_symlinks=False)
        shutil.copy(os.path.join(self.build_folder, "relocate-sdk.sh"), self.package_folder, follow_symlinks=False)
        shutil.copy(os.path.join(self.build_folder, "summary.csv"), self.package_folder, follow_symlinks=False)
        os.system(os.path.join(self.package_folder, "relocate-sdk.sh"))

        # The buildroot provided toolchain is not meant to be used with conan CMakeToolchain generator as we are trying to do here
        # because it sets CMAKE_FIND_ROOT_PATH variable to the toolchain sysroot. This is in general correct because the buildroot toolchain
        # should not search for libraries or modules elsewhere than the toolchain sysroot. In this case conan install module needed to find libraries on host paths
        # and this needs the CMAKE_FIND_ROOT_PATH set to the host root to work.
        replace_in_file(self, os.path.join(self.package_folder, "share", "buildroot", "toolchainfile.cmake"), "set(CMAKE_FIND_ROOT_PATH \"${RELOCATED_HOST_DIR}/x86_64-buildroot-linux-gnu/sysroot\")", "set(CMAKE_FIND_ROOT_PATH \"/\")")
        
        # Sometimes we have to find libraries within the host /usr/lib/architecture path. This is the case for example with SFML that relies on X11
        # being apt-getted by conan and installed in that path. Unluckily cmake's find_package doesn't search into the path unless the CMAKE_LIBRARY_ARCHITECTURE
        # is set. Buildroot toolchain doesn't do it on its own. 
        platformFile = open(os.path.join(self.package_folder, "share", "buildroot", "Platform", "Buildroot.cmake"), "a") 
        platformFile.write("set(CMAKE_LIBRARY_ARCHITECTURE \"x86_64-linux-gnu\")")
        platformFile.close()

        # Stupidest workaround in the world to solve CMakeDeps generator bug  when dealing with system packages (https://github.com/conan-io/conan/issues/11688)
        # Adding an host path to linker search path will 100% bite me in the ass but what can I do?
        cmake_c_prefix = "set(CMAKE_C_FLAGS \""
        cmake_c_postfix = "\" CACHE STRING \"Buildroot CFLAGS\")"
        cmake_c_flags = "-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Os -g0 -D_FORTIFY_SOURCE=1"
        cmake_c_new_flags = "-L/usr/lib/x86_64-linux-gnu -Wl,-rpath-link,/usr/lib/x86_64-linux-gnu"
        cmake_c_search = cmake_c_prefix + cmake_c_flags + cmake_c_postfix
        cmake_c_replace = cmake_c_prefix + cmake_c_flags + " " + cmake_c_new_flags + cmake_c_postfix
        
        replace_in_file(self, os.path.join(self.package_folder, "share", "buildroot", "toolchainfile.cmake"), cmake_c_search, cmake_c_replace)

        cmake_cxx_prefix = "set(CMAKE_CXX_FLAGS \""
        cmake_cxx_postfix = "\" CACHE STRING \"Buildroot CXXFLAGS\")"
        cmake_cxx_flags = "-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Os -g0 -D_FORTIFY_SOURCE=1"
        cmake_cxx_new_flags = "-L/usr/lib/x86_64-linux-gnu -Wl,-rpath-link,/usr/lib/x86_64-linux-gnu"
        cmake_cxx_search = cmake_cxx_prefix + cmake_cxx_flags + cmake_cxx_postfix
        cmake_cxx_replace = cmake_cxx_prefix + cmake_cxx_flags + " " + cmake_cxx_new_flags + cmake_cxx_postfix
        
        replace_in_file(self, os.path.join(self.package_folder, "share", "buildroot", "toolchainfile.cmake"), cmake_cxx_search, cmake_cxx_replace)


    def package_info(self):
        self.conf_info.append("tools.cmake.cmaketoolchain:user_toolchain", os.path.join(self.package_folder, "share", "buildroot", "toolchainfile.cmake"))
