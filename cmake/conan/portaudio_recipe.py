import os
from conans import ConanFile, CMake, AutoToolsBuildEnvironment, tools
from conans.tools import os_info, SystemPackageTool, download, untargz, replace_in_file, unzip

class PortAudioRecipe(ConanFile):
    name = "portaudio"
    version = "v190600.20161030"
    settings = "os", "compiler", "build_type", "arch"
    FOLDER_NAME = "portaudio"
    description = "Conan package for the Portaudio library"
    url = "https://github.com/zyperpl/zdlib"
    license = "http://www.portaudio.com/license.html"
    options = {"shared": [True, False], "fPIC": [True, False], "sysroot": "ANY" }
    default_options = "shared=False", "fPIC=True", "sysroot=''"

    WIN = {'build_dirname': "_build"}

    def configure(self):
        del self.settings.compiler.libcxx
        if self.settings.os == "Windows":
            self.options.remove("fPIC")

    def system_requirements(self):
        if os_info.is_linux:
            if os_info.with_apt:
                installer = SystemPackageTool()
                installer.install("libasound2-dev")
                installer.install("libjack-dev")
            elif os_info.with_yum:
                installer = SystemPackageTool()
                installer.install("alsa-lib-devel")
                installer.install("jack-audio-connection-kit-devel")

    def source(self):
        zip_name = 'portaudio_%s' % self.version
        if self.version == 'master':
            self.run('mkdir portaudio')
            zip_name += '.zip'
            download('https://app.assembla.com/spaces/portaudio/git/source/master?_format=zip', 'portaudio/%s' % zip_name)
            unzip('portaudio/%s' % zip_name, 'portaudio/')
            os.unlink('portaudio/%s' % zip_name)
        else:
            zip_name += '.tgz'
            download('http://portaudio.com/archives/pa_stable_%s.tgz' % self.version.replace('.','_'), zip_name)
            untargz(zip_name)
            os.unlink(zip_name)

        if self.settings.os != "Windows":
            self.run("chmod +x ./%s/configure" % self.FOLDER_NAME)

    def build(self):
        if self.settings.os == "Linux" or self.settings.os == "Macos":
            env = AutoToolsBuildEnvironment(self)
            with tools.environment_append(env.vars):
                env.fpic = self.options.fPIC
                configure_params = ""

                if (self.options.sysroot != ""):
                    env.vars['CFLAGS'] += ' -isysroot {} '.format(self.options.sysroot)
                    env.vars['CXXFLAGS'] += ' -isysroot {} '.format(self.options.sysroot)
                    env.vars['CFLAGS'] += ' -sysroot {} '.format(self.options.sysroot)
                    env.vars['LDFLAGS'] += ' -isysroot {} '.format(self.options.sysroot)
                    configure_params += ' --with-sysroot={} '.format(self.options.sysroot)

                env.vars['CFLAGS'] += ' -arch {}'.format(self.settings.arch)

                prepend_params = ""
                if self.settings.os == "Linux":
                    configure_params += "--host={}-pc-linux ".format(self.settings.arch)
                elif self.settings.os == "Macos":
                    env.vars['CFLAGS'] += ' -mmacosx-version-min=10.15 '
                    env.vars['CXXFLAGS'] += ' -mmacosx-version-min=10.15 '
                    env.vars['LDFLAGS'] += ' -mmacosx-version-min=10.15 '
                    configure_params += " --host={}-apple-darwin19 ".format(self.settings.arch)
                    configure_params += " --target={}-apple-darwin19 ".format(self.settings.arch)
                    configure_params += " --disable-mac-universal "
                    configure_params += " --without-alsa --without-jack --without-oss --without-asihpi "
                    env.vars['CC'] = 'o64-clang'
                    prepend_params = "CC=o64-clang AR=x86_64-apple-darwin19-ar "
                    configure_params += " AR={}/../../bin/x86_64-apple-darwin19-ar ".format(self.options.sysroot)

                with tools.environment_append(env.vars):
                    command = '{} ./configure {} && {} make VERBOSE=1'.format(prepend_params, configure_params, prepend_params)
                    print("Running {} command.".format(command))
                    self.run("cd %s && %s" % (self.FOLDER_NAME, command))
            if self.settings.os == "Macos" and self.options.shared:
                self.run('cd portaudio/lib/.libs && for filename in *.dylib; do install_name_tool -id $filename $filename; done')
        else:
            if self.settings.compiler == "gcc":
                replace_in_file(os.path.join(self.FOLDER_NAME, "CMakeLists.txt"), 'OPTION(PA_USE_WDMKS "Enable support for WDMKS" ON)', 'OPTION(PA_USE_WDMKS "Enable support for WDMKS" OFF)')
                replace_in_file(os.path.join(self.FOLDER_NAME, "CMakeLists.txt"), 'OPTION(PA_USE_WDMKS_DEVICE_INFO "Use WDM/KS API for device info" ON)', 'OPTION(PA_USE_WDMKS_DEVICE_INFO "Use WDM/KS API for device info" OFF)')
                replace_in_file(os.path.join(self.FOLDER_NAME, "CMakeLists.txt"), 'OPTION(PA_USE_WASAPI "Enable support for WASAPI" ON)', 'OPTION(PA_USE_WASAPI "Enable support for WASAPI" OFF)')

            build_dirname = self.WIN['build_dirname']

            cmake = CMake(self.settings)

            if self.settings.os == "Windows":
                self.run("IF not exist {} mkdir {}".format(build_dirname, build_dirname))
            else:
                self.run("mkdir {}".format(build_dirname))

            cmake_command = 'cd {} && cmake {} {}'.format(build_dirname, os.path.join("..", self.FOLDER_NAME), cmake.command_line)
            self.output.info(cmake_command)
            self.run(cmake_command)

            build_command = "cd {} && cmake --build . {}".format(build_dirname, cmake.build_config)
            self.output.info(build_command)
            self.run(build_command)

    def package(self):
        self.copy("*.h", dst="include", src=os.path.join(self.FOLDER_NAME, "include"))

        self.copy(pattern="LICENSE*", dst="licenses", src=self.FOLDER_NAME,  ignore_case=True, keep_path=False)
        
        if self.settings.os == "Windows":
            build_dirname = self.WIN['build_dirname']
            if self.settings.compiler == "Visual Studio":
                self.copy("*.lib", dst="lib", src=os.path.join(build_dirname, str(self.settings.build_type)))
                if self.options.shared:
                    self.copy("*.dll", dst="bin", src=os.path.join(build_dirname, str(self.settings.build_type)))
            else:
                if self.options.shared:
                    self.copy(pattern="*.dll.a", dst="lib", keep_path=False)
                    self.copy(pattern="*.dll", dst="bin", keep_path=False)
                else:
                    self.copy(pattern="*static.a", dst="lib", keep_path=False)
                
        else:
            if self.options.shared:
                if self.settings.os == "Macos":
                    self.copy(pattern="*.dylib", dst="lib", src=os.path.join(self.FOLDER_NAME, "lib", ".libs"))
                else:
                    self.copy(pattern="*.so*", dst="lib", src=os.path.join(self.FOLDER_NAME, "lib", ".libs"))
            else:
                self.copy("*.a", dst="lib", src=os.path.join(self.FOLDER_NAME, "lib", ".libs"))


    def package_info(self):
        base_name = "portaudio"
        if self.settings.os == "Windows":
            if not self.options.shared:
                base_name += "_static"
                
            if self.settings.compiler == "Visual Studio":
                base_name += "_x86" if self.settings.arch == "x86" else "_x64"
            
        elif self.settings.os == "Macos":
            self.cpp_info.exelinkflags.append("-framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices -framework Carbon")

        self.cpp_info.libs = [base_name]

        if self.settings.os == "Windows" and self.settings.compiler == "gcc" and not self.options.shared:
            self.cpp_info.libs.append('winmm')

        if self.settings.os == "Linux" and not self.options.shared:
            self.cpp_info.libs.append('jack asound m pthread')
