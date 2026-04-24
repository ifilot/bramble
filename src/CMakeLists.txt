 #*************************************************************************
 #   CMakeLists.txt  --  This file is part of BRAMBLE.                    *
 #                                                                        *
 #   Author: Ivo Filot <i.a.w.filot@tue.nl>                               *
 #                                                                        *
 #   BRAMBLE is free software: you can redistribute it and/or modify      *
 #   it under the terms of the GNU General Public License as published    *
 #   by the Free Software Foundation, either version 3 of the License,    *
 #   or (at your option) any later version.                               *
 #                                                                        *
 #   BRAMBLE is distributed in the hope that it will be useful,           *
 #   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 #   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 #   See the GNU General Public License for more details.                 *
 #                                                                        *
 #   You should have received a copy of the GNU General Public License    *
 #   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 #                                                                        *
 #*************************************************************************/

# set minimum cmake requirements
cmake_minimum_required(VERSION 3.18)
project (bramble)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BRAMBLE_NATIVE "Build with native CPU optimizations" OFF)

# change compiler directives when code coverage is required
if(USE_GCOV)
  set(CMAKE_BUILD_TYPE "Debug")

  # Set global c and c++ flags
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")

  # Link flags used for creating executables
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov -fprofile-arcs")

  # Link flags used for creating shared libraries
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lgcov -profile-arcs")
endif()

# store git hash
if(DEFINED ENV{GIT_HASH})
    set(GIT_HASH $ENV{GIT_HASH})
    MESSAGE(STATUS "GIT HASH externally defined: " ${GIT_HASH})
else()
    execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    MESSAGE(STATUS "GIT HASH grabbed from Git command: " GIT_HASH)
endif()

# prepare configuration file
SET(VERSION_MAJOR "1")
SET(VERSION_MINOR "2")
SET(VERSION_PATCH "0")
message(STATUS "Writing configuration file in: ${CMAKE_CURRENT_BINARY_DIR}/config.h")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

# Enable release build
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# add OS specific
SET(BOOST_INCLUDEDIR "/usr/include")
SET(BOOST_LIBRARYDIR "/usr/lib/x86_64-linux-gnu")

find_package(OpenMP)
if (OPENMP_FOUND)
    option(HAS_OPENMP "OpenMP enabled" ON)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# set Boost
set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS OFF)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK ON)

# Include libraries
find_package(PkgConfig REQUIRED)
find_package(Boost COMPONENTS regex unit_test_framework system REQUIRED)
pkg_check_modules(TCLAP tclap REQUIRED)
pkg_check_modules(EIGEN eigen3 REQUIRED)

# Set include folders
include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${TCLAP_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS}
                    ${EIGEN_INCLUDE_DIRS})

# enable CUDA support
if(MOD_CUDA)
    if(DEFINED CUDA_ARCH AND NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
        string(REPLACE "sm_" "" CUDA_ARCHITECTURES "${CUDA_ARCH}")
        string(REPLACE "compute_" "" CUDA_ARCHITECTURES "${CUDA_ARCHITECTURES}")
        string(REPLACE "," ";" CUDA_ARCHITECTURES "${CUDA_ARCHITECTURES}")
        set(CMAKE_CUDA_ARCHITECTURES "${CUDA_ARCHITECTURES}")
    elseif(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
        if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.23)
            set(CMAKE_CUDA_ARCHITECTURES "all-major")
        else()
            set(CMAKE_CUDA_ARCHITECTURES "75;80;90")
        endif()
    endif()

    enable_language(CUDA)
    find_package(CUDAToolkit 11.0 REQUIRED)

    message(STATUS "CUDA architectures: ${CMAKE_CUDA_ARCHITECTURES}")

    file(GLOB CUDA_FILES "*.cu")
    add_library(cuda_files STATIC ${CUDA_FILES})
    set_target_properties(cuda_files PROPERTIES
                          CUDA_SEPARABLE_COMPILATION ON
                          CUDA_STANDARD 17
                          CUDA_STANDARD_REQUIRED ON)
    target_link_libraries(cuda_files PUBLIC CUDA::cudart)

    # create cuda test executable for probing cards
    add_executable(bramblecuda ${CMAKE_CURRENT_SOURCE_DIR}/bramblecuda.cpp)
    target_link_libraries(bramblecuda cuda_files)

    # set compile directive
    add_compile_definitions(MOD_CUDA)
endif()

# Add sources
file(GLOB SOURCES "*.cpp")
list(REMOVE_ITEM
     SOURCES
     ${CMAKE_CURRENT_SOURCE_DIR}/bramble.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/brambletool.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/bramblecuda.cpp)

add_library(bramblesources STATIC ${SOURCES})

add_executable(bramble ${CMAKE_CURRENT_SOURCE_DIR}/bramble.cpp)
add_executable(brambletool ${CMAKE_CURRENT_SOURCE_DIR}/brambletool.cpp)

set(BRAMBLE_TARGETS bramblesources bramble brambletool)
if(MOD_CUDA)
    list(APPEND BRAMBLE_TARGETS bramblecuda)
endif()

foreach(BRAMBLE_TARGET ${BRAMBLE_TARGETS})
    target_compile_features(${BRAMBLE_TARGET} PRIVATE cxx_std_17)
    if(BRAMBLE_NATIVE)
        target_compile_options(${BRAMBLE_TARGET} PRIVATE -march=native)
    endif()
endforeach()

# Link libraries
if(MOD_CUDA)
    target_link_libraries(bramble
                          bramblesources
                          cuda_files
                          ${Boost_LIBRARIES})
else()
    target_link_libraries(bramble
                          bramblesources
                          ${Boost_LIBRARIES})
endif()

target_link_libraries(brambletool
                      bramblesources
                      ${Boost_LIBRARIES})

if(NOT DISABLE_TEST)
    include(CTest)
    message("[USER] Performing unit tests")
    enable_testing()
    add_subdirectory("test")
else()
    message("[USER] Testing routine disabled")
endif()

###
# Installing
##
install (TARGETS bramble DESTINATION bin)
install (TARGETS brambletool DESTINATION bin)

if(MOD_CUDA)
    install (TARGETS bramblecuda DESTINATION bin)
endif()
