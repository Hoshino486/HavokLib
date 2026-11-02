cmake_minimum_required(VERSION 3.13)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(FATAL_ERROR "X86 architecture is not supported!")
endif()

project(HavokLib VERSION 1)

find_package(Python3 COMPONENTS Development Interpreter)

# ~~~
# Valid targets:
#  - havok-objects: static objects target (enabled by default, set NO_OBJECTS to disable all)
#  - havok: shared library target (BUILD_SHARED_LIBS == ON)
# ~~~

option(PYTHON_MODULE "Build a module for Python 3 (enables OBJECTS_PID)." ON)
option(TOOLSET "Build toolset." ON)
option(ODR_TEST "Enable ODR testing." OFF)

if(NOT Python3_FOUND AND PYTHON_MODULE)
  message(FATAL_ERROR "Python3 not found.")
endif()

option(OBJECTS_PID "Imply PID for all objects." ${PYTHON_MODULE})

option(NO_OBJECTS "" OFF)
option(CLI "" ${TOOLSET})
option(GLTF "" ${TOOLSET})

set(CMAKE_CXX_STANDARD 20)

# ~~~ Compiler flavour flags ~~~
# The upstream project is written against GCC/Clang. We translate the flag sets
# here so the tree also configures and builds with MSVC (cl.exe).
if(MSVC)
  set(HK_WARN_FLAGS /W3)
  # -fvisibility=hidden has no MSVC equivalent (that is the default for DLLs)
  set(HK_VISIBILITY_FLAGS )
  # -msse4: x64 MSVC has SSE2 baseline, /arch:AVX enables the rest of the
  # SIMD helper paths used by Spike.
  set(HK_SIMD_FLAGS /arch:AVX)
  # -Wno-invalid-offsetof / -fno-rtti: RTTI stays on for MSVC builds.
  set(HK_CXX_ONLY_FLAGS )
  # -Wl,/stack:8388608 -> /STACK:8388608
  set(HK_STACK_LINK_FLAGS /STACK:8388608)
  set(HK_RDYNAMIC_FLAGS )
  add_compile_options(/utf-8 /bigobj /Zc:__cplusplus /Zc:preprocessor
                      /permissive- /wd4267 /wd4244 /wd4996 /wd4100 /wd4189)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
else()
  set(HK_WARN_FLAGS -Wall -Wextra)
  set(HK_VISIBILITY_FLAGS -fvisibility=hidden)
  set(HK_SIMD_FLAGS -msse4)
  set(HK_CXX_ONLY_FLAGS -Wno-invalid-offsetof -fno-rtti)
  set(HK_STACK_LINK_FLAGS )
  set(HK_RDYNAMIC_FLAGS -rdynamic)
endif()

add_compile_options(${HK_WARN_FLAGS})

if(TOOLSET)
  set(EXPOSE_SYMBOLS spike;gltf;havok;pugixml)
endif()

set(TPD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party)
add_subdirectory(${TPD_PATH}/spike)
include(targetex)
add_subdirectory(classgen)
add_subdirectory(source)

if(TOOLSET)
  add_subdirectory(toolset)
endif()

if(PYTHON_MODULE)
  add_subdirectory(python)
endif()

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  enable_testing()
  add_subdirectory(test)
endif()
