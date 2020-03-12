CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

project("zd" CXX)
SET(NAME "zdtest")

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

find_program(CCACHE ccache)
if(CCACHE)
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else()
  message("ccache cannot be found")
endif()

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release")
ENDIF()
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES("src/ZD/" "src/")

ADD_DEFINITIONS(-DNAME="${NAME}")

set(SANITIZERS "")
option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" FALSE)
if(ENABLE_SANITIZER_ADDRESS)
  list(APPEND SANITIZERS "address")
endif()

option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" FALSE)
if(ENABLE_SANITIZER_MEMORY)
  list(APPEND SANITIZERS "memory")
endif()

option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
        "Enable undefined behavior sanitizer" FALSE)
if(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
  list(APPEND SANITIZERS "undefined")
endif()

option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" FALSE)
if(ENABLE_SANITIZER_THREAD)
  list(APPEND SANITIZERS "thread")
endif()
list(JOIN SANITIZERS "," LIST_OF_SANITIZERS)
set(SANITIZERS_STR "")
if(NOT "${LIST_OF_SANITIZERS}" STREQUAL "")
  set(SANITIZERS_STR "-fsanitize=${LIST_OF_SANITIZERS}")
endif()

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb ${SANITIZERS_STR} -fno-omit-frame-pointer -Wall -Wextra -DDEBUG -O0")
SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -O3 -s -DNDEBUG")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
add_compile_options(${CMAKE_CXX_FLAGS} -fopenmp -std=c++2a -Wall)

SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build/")

set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

include(cmake/conan/ConanRun.cmake)
run_conan()

find_library(OpenMP CONFIG REQUIRED)
find_library(glfw3 CONFIG REQUIRED)
find_library(glew CONFIG REQUIRED)
INCLUDE_DIRECTORIES (${GLFW_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${GLEW_INCLUDE_DIR})

FILE(GLOB SOURCES "src/*.cpp" "src/ZD/*.cpp")
ADD_EXECUTABLE(${NAME} "${SOURCES}")
TARGET_LINK_LIBRARIES(${NAME} PRIVATE ${CONAN_LIB_DIRS_OPENMP}/libomp.a stdc++ CONAN_PKG::glew CONAN_PKG::glfw CONAN_PKG::portaudio dl pthread)

add_custom_target(run
  COMMAND ./build/${NAME}
  DEPENDS ${NAME}
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

