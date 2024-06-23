from conan import ConanFile
from conan.tools.files import get, replace_in_file
import shutil
import os

recipe_arch = "armv7hf"
toolchain_arch = "arm"
download_toolchain_arch = "armv7-eabihf"
toolchain_distro = "linux-gnueabihf"

class Toolchain(ConanFile):
    name = "toolchain-linux-" + recipe_arch + "-gcc-11.3"
    version = "1.0"

    def source(self):
        get(self, "https://toolchains.bootlin.com/downloads/releases/toolchains/" + download_toolchain_arch + "/tarballs/" + download_toolchain_arch + "--glibc--stable-2022.08-1.tar.bz2",
                  strip_root=True)

    def package(self):
        toolchain_base_folder = toolchain_arch + "-buildroot-" + toolchain_distro

        shutil.copytree(os.path.join(self.build_folder, "bin"), os.path.join(self.package_folder, "bin"), dirs_exist_ok=True, symlinks=True)
        shutil.copytree(os.path.join(self.build_folder, "etc"), os.path.join(self.package_folder, "etc"), dirs_exist_ok=True, symlinks=True)
        shutil.copytree(os.path.join(self.build_folder, "include"), os.path.join(self.package_folder, "include"), dirs_exist_ok=True, symlinks=True)
        shutil.copytree(os.path.join(self.build_folder, "lib"), os.path.join(self.package_folder, "lib"), dirs_exist_ok=True, symlinks=True)
        shutil.copytree(os.path.join(self.build_folder, "libexec"), os.path.join(self.package_folder, "libexec"), dirs_exist_ok=True, symlinks=True)
        shutil.copytree(os.path.join(self.build_folder, "share"), os.path.join(self.package_folder, "share"), dirs_exist_ok=True, symlinks=True)
        shutil.copytree(os.path.join(self.build_folder, toolchain_base_folder), os.path.join(self.package_folder, toolchain_base_folder), dirs_exist_ok=True, symlinks=True)
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
        replace_find_root = "set(CMAKE_FIND_ROOT_PATH \"${RELOCATED_HOST_DIR}/"
        replace_find_root = replace_find_root + toolchain_base_folder + "/sysroot\")"
        replace_in_file(self, os.path.join(self.package_folder, "share", "buildroot", "toolchainfile.cmake"), replace_find_root, "set(CMAKE_FIND_ROOT_PATH \"/\")")

        # Sometimes we have to find libraries within the host /usr/lib/architecture path. This is the case for example with SFML that relies on X11
        # being apt-getted by conan and installed in that path. Unluckily cmake's find_package doesn't search into the path unless the CMAKE_LIBRARY_ARCHITECTURE
        # is set. Buildroot toolchain doesn't do it on its own.
        platformFile = open(os.path.join(self.package_folder, "share", "buildroot", "Platform", "Buildroot.cmake"), "a")
        platformFile.write(f"set(CMAKE_LIBRARY_ARCHITECTURE \"{toolchain_arch}-{toolchain_distro}\")")
        platformFile.close()

        # Stupidest workaround in the world to solve CMakeDeps generator bug  when dealing with system packages (https://github.com/conan-io/conan/issues/11688)
        # Adding an host path to linker search path will 100% bite me in the ass but what can I do?
        external_folder = os.path.join(self.package_folder, toolchain_base_folder, "sysroot", "usr", "lib", "external")
        source_folder = os.path.join("/usr", "lib", f"{toolchain_arch}-{toolchain_distro}")
        os.makedirs(external_folder)

        file_names = [
            "libGL.so", "libSM.so", "libSM.a", "libuuid.so", "libuuid.a",
            "libxcb-cursor.so", "libxcb-cursor.a", "libxcb-dri3.so", "libxcb-dri3.a",
            "libxcb-util.so", "libxcb-util.a", "libxcb-xinerama.so", "libxcb-xinerama.a",
            "libxcb-xfixes.so", "libxcb-xfixes.a", "libxcb-sync.so", "libxcb-sync.a",
            "libxcb-shape.so", "libxcb-shape.a", "libxcb-render-util.so", "libxcb-render-util.a",
            "libxcb-render.so", "libxcb-render.a", "libxcb-randr.so", "libxcb-randr.a",
            "libxcb-keysyms.so", "libxcb-res.so", "libxcb-ewmh.so", "libxcb-composite.so",
            "libxcb-present.so", "libxcb-glx.so", "libxcb-dri2.so", "libxcb-keysyms.a",
            "libxcb-image.so", "libxcb-image.a", "libxcb-shm.so", "libxcb-shm.a",
            "libxcb-icccm.so", "libxcb-icccm.a", "libxcb-xkb.so", "libxcb-xkb.a",
            "libXxf86vm.so", "libXxf86vm.a", "libXvMC.so", "libXvMC.a",
            "libXv.so", "libXv.a", "libXtst.so", "libXtst.a",
            "libXss.so", "libXss.a", "libXRes.so", "libXRes.a",
            "libXrender.so", "libXrender.a", "libXrandr.so", "libXrandr.a",
            "libXpm.so", "libXpm.a", "libXmuu.so", "libXmuu.a",
            "libXmu.so", "libXmu.a", "libxkbfile.so", "libxkbfile.a",
            "libXinerama.so", "libXinerama.a", "libXi.so", "libXi.a",
            "libXext.so", "libXext.a", "libXdmcp.so", "libXdmcp.a",
            "libXdamage.so", "libXdamage.a", "libXfixes.so", "libXfixes.a",
            "libXcursor.so", "libXcursor.a", "libXcomposite.so", "libXcomposite.a",
            "libXaw7.so", "libXaw7.a", "libXt.so", "libXt.a",
            "libXau.so", "libXau.a", "libICE.so", "libICE.a",
            "libfontenc.so", "libfontenc.a", "libX11-xcb.so", "libX11-xcb.a",
            "libxcb.so", "libxcb.a", "libX11.so", "libX11.a",
            "libudev.so", "libGLdispatch.so.0", "libGLX.so.0", "libbsd.so.0",
            "libz.so.1", "libmd.so.0"
        ]

        for file_name in file_names:
            source_path = os.path.join(source_folder, file_name)
            external_path = os.path.join(external_folder, file_name)
            os.symlink(source_path, external_path)

        include_folder = os.path.join(self.package_folder, toolchain_base_folder, "sysroot", "usr", "include")
        external_include_folder = os.path.join(self.package_folder, toolchain_base_folder, "include")

        os.symlink(os.path.join("/usr", "include", "pulse"), os.path.join(include_folder, "pulse"))
        os.symlink(os.path.join("/usr", "include", "dbus-1.0", "dbus"), os.path.join(include_folder, "dbus"))
        os.symlink(os.path.join("/usr", "lib", f"{toolchain_arch}-{toolchain_distro}", "dbus-1.0", "include", "dbus"), os.path.join(external_include_folder, "dbus"))

        cmake_c_prefix = "set(CMAKE_C_FLAGS \""
        cmake_c_postfix = "\" CACHE STRING \"Buildroot CFLAGS\")"
        cmake_c_flags = "-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Os -g0 -D_FORTIFY_SOURCE=1"
        cmake_c_new_flags = "-L" + external_folder + " -Wl,-rpath-link," + external_folder
        cmake_c_search = cmake_c_prefix + cmake_c_flags + cmake_c_postfix
        cmake_c_replace = cmake_c_prefix + cmake_c_flags + " " + cmake_c_new_flags + cmake_c_postfix

        replace_in_file(self, os.path.join(self.package_folder, "share", "buildroot", "toolchainfile.cmake"), cmake_c_search, cmake_c_replace)

        cmake_cxx_prefix = "set(CMAKE_CXX_FLAGS \""
        cmake_cxx_postfix = "\" CACHE STRING \"Buildroot CXXFLAGS\")"
        cmake_cxx_flags = "-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Os -g0 -D_FORTIFY_SOURCE=1"
        cmake_cxx_new_flags = "-L" + os.path.join(self.package_folder, toolchain_base_folder, "sysroot", "usr", "lib", "external") + " -Wl,-rpath-link," + os.path.join(self.package_folder, toolchain_base_folder, "sysroot", "usr", "lib", "external")
        cmake_cxx_search = cmake_cxx_prefix + cmake_cxx_flags + cmake_cxx_postfix
        cmake_cxx_replace = cmake_cxx_prefix + cmake_cxx_flags + " " + cmake_cxx_new_flags + cmake_cxx_postfix

        replace_in_file(self, os.path.join(self.package_folder, "share", "buildroot", "toolchainfile.cmake"), cmake_cxx_search, cmake_cxx_replace)

    def package_info(self):
        compiler_cxx = toolchain_arch + "-buildroot-" + toolchain_distro + "-g++"
        compiler_c = toolchain_arch + "-buildroot-" + toolchain_distro + "-gcc"
        compiler_path = os.path.join(self.package_folder, "bin")
        self.conf_info.append("tools.cmake.cmaketoolchain:user_toolchain", os.path.join(self.package_folder, "share", "buildroot", "toolchainfile.cmake"))
        self.conf_info.define("tools.build:compiler_executables", {"c": os.path.join(compiler_path, compiler_c), "cpp": os.path.join(compiler_path, compiler_cxx)})
