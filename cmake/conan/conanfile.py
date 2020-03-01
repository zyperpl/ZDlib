import os

from conans import ConanFile

class ZDConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    url = "https://github.com/zyperpl"

    def include_package(self, name, version, recipe=None):
        recipes_path = os.path.join("../cmake/conan/", ".")
        if recipe is None:
            recipe = name + "_recipe.py"
            
        self.run("conan export %s zd/stable" % recipe, cwd=recipes_path)
        self.requires(("%s/%s@zd/stable" % (name, version)))

    def requirements(self):
        self.include_package("glew", "2.1.0")
        self.include_package("glfw", "3.3.2")
        self.include_package("openmp", "latest")
        self.include_package("portaudio", "v190600.20161030")
