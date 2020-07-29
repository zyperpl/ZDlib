from conans import ConanFile, tools, CMake
import os
import shutil

class OpenMPConan(ConanFile):
    name = "openmp"
    version = "latest"
    description = "LLVM Library for parallel programming"
    homepage = "https://github.com/llvm-mirror/openmp"
    url = "https://github.com/zyperpl/zdlib/cmake/conan"
    license = "Apache"
    author = "Zyper (github.com/zyperpl)"
    generators = "cmake"
    settings = "os", "arch", "compiler", "build_type"
    options = { "shared": [True, False], "fPIC": [True, False] }
    default_options = { "shared": False, "fPIC": True }
    exports = "LICENSE.txt"
    short_paths=True
    
    _source_folder = "openmp"
    _build_folder = "{0}-{1}_build".format(name, version)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        del self.settings.compiler.libcxx

    def source(self):
        self.run("git clone {}".format(self.homepage))

    def build(self):
        cmake = CMake(self)
        cmake.definitions["LIBOMP_ENABLE_SHARED"] = False
        cmake.configure(source_folder=self._source_folder, build_folder=self._build_folder)
        cmake.build()
        cmake.install()

    def package(self):
        self.copy(pattern="*.pdb", dst="bin", keep_path=False)        
        for export in self.exports:
            self.copy(export, keep_path=False)

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        if self.settings.os == "Linux":
            if not self.options.shared:
                self.cpp_info.libs.append('pthread')
