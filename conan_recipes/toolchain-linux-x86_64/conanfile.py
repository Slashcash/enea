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
        external_folder = os.path.join(self.package_folder, "x86_64-buildroot-linux-gnu", "sysroot", "usr", "lib", "external")
        source_folder = os.path.join("/usr", "lib", "x86_64-linux-gnu")
        os.makedirs(external_folder)
        os.symlink(os.path.join(source_folder, "libGL.so"), os.path.join(external_folder, "libGL.so"))
        os.symlink(os.path.join(source_folder, "libSM.so"), os.path.join(external_folder, "libSM.so"))
        os.symlink(os.path.join(source_folder, "libSM.a"), os.path.join(external_folder, "libSM.a"))
        os.symlink(os.path.join(source_folder, "libuuid.so"), os.path.join(external_folder, "libuuid.so"))
        os.symlink(os.path.join(source_folder, "libuuid.a"), os.path.join(external_folder, "libuuid.a"))
        os.symlink(os.path.join(source_folder, "libxcb-cursor.so"), os.path.join(external_folder, "libxcb-cursor.so"))
        os.symlink(os.path.join(source_folder, "libxcb-cursor.a"), os.path.join(external_folder, "libxcb-cursor.a"))
        os.symlink(os.path.join(source_folder, "libxcb-dri3.so"), os.path.join(external_folder, "libxcb-dri3.so"))
        os.symlink(os.path.join(source_folder, "libxcb-dri3.a"), os.path.join(external_folder, "libxcb-dri3.a"))
        os.symlink(os.path.join(source_folder, "libxcb-util.so"), os.path.join(external_folder, "libxcb-util.so"))
        os.symlink(os.path.join(source_folder, "libxcb-util.a"), os.path.join(external_folder, "libxcb-util.a"))
        os.symlink(os.path.join(source_folder, "libxcb-xinerama.so"), os.path.join(external_folder, "libxcb-xinerama.so"))
        os.symlink(os.path.join(source_folder, "libxcb-xinerama.a"), os.path.join(external_folder, "libxcb-xinerama.a"))
        os.symlink(os.path.join(source_folder, "libxcb-xfixes.so"), os.path.join(external_folder, "libxcb-xfixes.so"))
        os.symlink(os.path.join(source_folder, "libxcb-xfixes.a"), os.path.join(external_folder, "libxcb-xfixes.a"))
        os.symlink(os.path.join(source_folder, "libxcb-sync.so"), os.path.join(external_folder, "libxcb-sync.so"))
        os.symlink(os.path.join(source_folder, "libxcb-sync.a"), os.path.join(external_folder, "libxcb-sync.a"))
        os.symlink(os.path.join(source_folder, "libxcb-shape.so"), os.path.join(external_folder, "libxcb-shape.so"))
        os.symlink(os.path.join(source_folder, "libxcb-shape.a"), os.path.join(external_folder, "libxcb-shape.a"))
        os.symlink(os.path.join(source_folder, "libxcb-render-util.so"), os.path.join(external_folder, "libxcb-render-util.so"))
        os.symlink(os.path.join(source_folder, "libxcb-render-util.a"), os.path.join(external_folder, "libxcb-render-util.a"))
        os.symlink(os.path.join(source_folder, "libxcb-render.so"), os.path.join(external_folder, "libxcb-render.so"))
        os.symlink(os.path.join(source_folder, "libxcb-render.a"), os.path.join(external_folder, "libxcb-render.a"))
        os.symlink(os.path.join(source_folder, "libxcb-randr.so"), os.path.join(external_folder, "libxcb-randr.so"))
        os.symlink(os.path.join(source_folder, "libxcb-randr.a"), os.path.join(external_folder, "libxcb-randr.a"))
        os.symlink(os.path.join(source_folder, "libxcb-keysyms.so"), os.path.join(external_folder, "libxcb-keysyms.so"))
        os.symlink(os.path.join(source_folder, "libxcb-keysyms.a"), os.path.join(external_folder, "libxcb-keysyms.a"))
        os.symlink(os.path.join(source_folder, "libxcb-image.so"), os.path.join(external_folder, "libxcb-image.so"))
        os.symlink(os.path.join(source_folder, "libxcb-image.a"), os.path.join(external_folder, "libxcb-image.a"))
        os.symlink(os.path.join(source_folder, "libxcb-shm.so"), os.path.join(external_folder, "libxcb-shm.so"))
        os.symlink(os.path.join(source_folder, "libxcb-shm.a"), os.path.join(external_folder, "libxcb-shm.a"))
        os.symlink(os.path.join(source_folder, "libxcb-icccm.so"), os.path.join(external_folder, "libxcb-icccm.so"))
        os.symlink(os.path.join(source_folder, "libxcb-icccm.a"), os.path.join(external_folder, "libxcb-icccm.a"))
        os.symlink(os.path.join(source_folder, "libxcb-xkb.so"), os.path.join(external_folder, "libxcb-xkb.so"))
        os.symlink(os.path.join(source_folder, "libxcb-xkb.a"), os.path.join(external_folder, "libxcb-xkb.a"))
        os.symlink(os.path.join(source_folder, "libXxf86vm.so"), os.path.join(external_folder, "libXxf86vm.so"))
        os.symlink(os.path.join(source_folder, "libXxf86vm.a"), os.path.join(external_folder, "libXxf86vm.a"))
        os.symlink(os.path.join(source_folder, "libXvMC.so"), os.path.join(external_folder, "libXvMC.so"))
        os.symlink(os.path.join(source_folder, "libXvMC.a"), os.path.join(external_folder, "libXvMC.a"))
        os.symlink(os.path.join(source_folder, "libXv.so"), os.path.join(external_folder, "libXv.so"))
        os.symlink(os.path.join(source_folder, "libXv.a"), os.path.join(external_folder, "libXv.a"))
        os.symlink(os.path.join(source_folder, "libXtst.so"), os.path.join(external_folder, "libXtst.so"))
        os.symlink(os.path.join(source_folder, "libXtst.a"), os.path.join(external_folder, "libXtst.a"))
        os.symlink(os.path.join(source_folder, "libXss.so"), os.path.join(external_folder, "libXss.so"))
        os.symlink(os.path.join(source_folder, "libXss.a"), os.path.join(external_folder, "libXss.a"))
        os.symlink(os.path.join(source_folder, "libXRes.so"), os.path.join(external_folder, "libXRes.so"))
        os.symlink(os.path.join(source_folder, "libXRes.a"), os.path.join(external_folder, "libXRes.a"))
        os.symlink(os.path.join(source_folder, "libXrender.so"), os.path.join(external_folder, "libXrender.so"))
        os.symlink(os.path.join(source_folder, "libXrender.a"), os.path.join(external_folder, "libXrender.a"))
        os.symlink(os.path.join(source_folder, "libXrandr.so"), os.path.join(external_folder, "libXrandr.so"))
        os.symlink(os.path.join(source_folder, "libXrandr.a"), os.path.join(external_folder, "libXrandr.a"))
        os.symlink(os.path.join(source_folder, "libXpm.so"), os.path.join(external_folder, "libXpm.so"))
        os.symlink(os.path.join(source_folder, "libXpm.a"), os.path.join(external_folder, "libXpm.a"))
        os.symlink(os.path.join(source_folder, "libXmuu.so"), os.path.join(external_folder, "libXmuu.so"))
        os.symlink(os.path.join(source_folder, "libXmuu.a"), os.path.join(external_folder, "libXmuu.a"))
        os.symlink(os.path.join(source_folder, "libXmu.so"), os.path.join(external_folder, "libXmu.so"))
        os.symlink(os.path.join(source_folder, "libXmu.a"), os.path.join(external_folder, "libXmu.a"))
        os.symlink(os.path.join(source_folder, "libxkbfile.so"), os.path.join(external_folder, "libxkbfile.so"))
        os.symlink(os.path.join(source_folder, "libxkbfile.a"), os.path.join(external_folder, "libxkbfile.a"))
        os.symlink(os.path.join(source_folder, "libXinerama.so"), os.path.join(external_folder, "libXinerama.so"))
        os.symlink(os.path.join(source_folder, "libXinerama.a"), os.path.join(external_folder, "libXinerama.a"))
        os.symlink(os.path.join(source_folder, "libXi.so"), os.path.join(external_folder, "libXi.so"))
        os.symlink(os.path.join(source_folder, "libXi.a"), os.path.join(external_folder, "libXi.a"))
        os.symlink(os.path.join(source_folder, "libXext.so"), os.path.join(external_folder, "libXext.so"))
        os.symlink(os.path.join(source_folder, "libXext.a"), os.path.join(external_folder, "libXext.a"))
        os.symlink(os.path.join(source_folder, "libXdmcp.so"), os.path.join(external_folder, "libXdmcp.so"))
        os.symlink(os.path.join(source_folder, "libXdmcp.a"), os.path.join(external_folder, "libXdmcp.a"))
        os.symlink(os.path.join(source_folder, "libXdamage.so"), os.path.join(external_folder, "libXdamage.so"))
        os.symlink(os.path.join(source_folder, "libXdamage.a"), os.path.join(external_folder, "libXdamage.a"))
        os.symlink(os.path.join(source_folder, "libXfixes.so"), os.path.join(external_folder, "libXfixes.so"))
        os.symlink(os.path.join(source_folder, "libXfixes.a"), os.path.join(external_folder, "libXfixes.a"))
        os.symlink(os.path.join(source_folder, "libXcursor.so"), os.path.join(external_folder, "libXcursor.so"))
        os.symlink(os.path.join(source_folder, "libXcursor.a"), os.path.join(external_folder, "libXcursor.a"))
        os.symlink(os.path.join(source_folder, "libXcomposite.so"), os.path.join(external_folder, "libXcomposite.so"))
        os.symlink(os.path.join(source_folder, "libXcomposite.a"), os.path.join(external_folder, "libXcomposite.a"))
        os.symlink(os.path.join(source_folder, "libXaw7.so"), os.path.join(external_folder, "libXaw7.so"))
        os.symlink(os.path.join(source_folder, "libXaw7.a"), os.path.join(external_folder, "libXaw7.a"))
        os.symlink(os.path.join(source_folder, "libXt.so"), os.path.join(external_folder, "libXt.so"))
        os.symlink(os.path.join(source_folder, "libXt.a"), os.path.join(external_folder, "libXt.a"))
        os.symlink(os.path.join(source_folder, "libXau.so"), os.path.join(external_folder, "libXau.so"))
        os.symlink(os.path.join(source_folder, "libXau.a"), os.path.join(external_folder, "libXau.a"))
        os.symlink(os.path.join(source_folder, "libICE.so"), os.path.join(external_folder, "libICE.so"))
        os.symlink(os.path.join(source_folder, "libICE.a"), os.path.join(external_folder, "libICE.a"))
        os.symlink(os.path.join(source_folder, "libfontenc.so"), os.path.join(external_folder, "libfontenc.so"))
        os.symlink(os.path.join(source_folder, "libfontenc.a"), os.path.join(external_folder, "libfontenc.a"))
        os.symlink(os.path.join(source_folder, "libX11-xcb.so"), os.path.join(external_folder, "libX11-xcb.so"))
        os.symlink(os.path.join(source_folder, "libX11-xcb.a"), os.path.join(external_folder, "libX11-xcb.a"))
        os.symlink(os.path.join(source_folder, "libxcb.so"), os.path.join(external_folder, "libxcb.so"))
        os.symlink(os.path.join(source_folder, "libxcb.a"), os.path.join(external_folder, "libxcb.a"))
        os.symlink(os.path.join(source_folder, "libX11.so"), os.path.join(external_folder, "libX11.so"))
        os.symlink(os.path.join(source_folder, "libX11.a"), os.path.join(external_folder, "libX11.a"))
        os.symlink(os.path.join(source_folder, "libudev.so"), os.path.join(external_folder, "libudev.so"))
        os.symlink(os.path.join(source_folder, "libGLdispatch.so.0"), os.path.join(external_folder, "libGLdispatch.so.0"))
        os.symlink(os.path.join(source_folder, "libGLX.so.0"), os.path.join(external_folder, "libGLX.so.0"))
        os.symlink(os.path.join(source_folder, "libbsd.so.0"), os.path.join(external_folder, "libbsd.so.0"))
        os.symlink(os.path.join(source_folder, "libz.so.1"), os.path.join(external_folder, "libz.so.1"))
        os.symlink(os.path.join(source_folder, "libmd.so.0"), os.path.join(external_folder, "libmd.so.0"))

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
        cmake_cxx_new_flags = "-L" + os.path.join(self.package_folder, "x86_64-buildroot-linux-gnu", "sysroot", "usr", "lib", "external") + " -Wl,-rpath-link," + os.path.join(self.package_folder, "x86_64-buildroot-linux-gnu", "sysroot", "usr", "lib", "external")
        cmake_cxx_search = cmake_cxx_prefix + cmake_cxx_flags + cmake_cxx_postfix
        cmake_cxx_replace = cmake_cxx_prefix + cmake_cxx_flags + " " + cmake_cxx_new_flags + cmake_cxx_postfix
        
        replace_in_file(self, os.path.join(self.package_folder, "share", "buildroot", "toolchainfile.cmake"), cmake_cxx_search, cmake_cxx_replace)

        # END OF STUPID WORKAROUND

    def package_info(self):
        self.conf_info.append("tools.cmake.cmaketoolchain:user_toolchain", os.path.join(self.package_folder, "share", "buildroot", "toolchainfile.cmake"))
