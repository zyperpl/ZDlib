macro(run_conan)
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(
    STATUS
      "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
       "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

set(ENV{CONAN_CMAKE_TOOLCHAIN_FILE} ${CMAKE_TOOLCHAIN_FILE})
set(ENV{CONAN_CMAKE_GENERATOR} ${CMAKE_GENERATOR})
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

conan_cmake_run(
  CONANFILE cmake/conan/conanfile.py
  OPTIONS
  ${CONAN_EXTRA_OPTIONS}
  BASIC_SETUP
  NO_OUTPUT_DIRS
  CMAKE_TARGETS
  BUILD
  missing)
endmacro()
