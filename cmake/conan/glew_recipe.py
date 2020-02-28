from conans import ConanFile, tools, CMake
import os
import shutil

class GlewConan(ConanFile):
    name = "glew"
    version = "2.1.0"
    description = "The OpenGL Extension Wrangler Library"
    homepage = "https://github.com/nigels-com/glew"
    url = "https://github.com/zyperpl"
    license = "MIT"
    author = "MANCIAUX Romain (https://github.com/PamplemousseMR), Zyper (github.com/zyperpl)"
    generators = "cmake"
    settings = "os", "arch", "compiler", "build_type"
    options = { "shared": [True, False], "fPIC": [True, False] }
    default_options = { "shared": False, "fPIC": True }
    exports = "LICENSE.md"
    short_paths=True
    
    _source_folder = "{0}-{1}_sources".format(name, version)
    _build_folder = "{0}-{1}_build".format(name, version)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        del self.settings.compiler.libcxx

    def source(self):
        tools.get("{0}/releases/download/{1}-{2}/{1}-{2}.tgz".format(self.homepage, self.name, self.version), sha256="04de91e7e6763039bc11940095cd9c7f880baba82196a7765f727ac05a993c95")
        os.rename("{0}-{1}".format(self.name, self.version), self._source_folder)

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=self._source_folder+"/build/cmake", build_folder=self._build_folder)
        cmake.build()
        cmake.install()

    def package(self):
        self.copy(pattern="*.pdb", dst="bin", keep_path=False)        
        for export in self.exports:
            self.copy(export, keep_path=False)

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        if self.settings.os == "Windows":
            if not self.options.shared:
                self.cpp_info.defines.append("GLEW_STATIC")
                self.cpp_info.libs.append('opengl32')
        elif self.settings.os == "Linux":
            if not self.options.shared:
                self.cpp_info.libs.append('GL')
        elif self.settings.os == "Macos":
            self.cpp_info.sharedlinkflags.append("-framework OpenGL")
