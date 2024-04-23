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
SET(VERSION_MINOR "1")
SET(VERSION_PATCH "3")
message(STATUS "Writing configuration file in: ${CMAKE_CURRENT_SOURCE_DIR}/config.h")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h @ONLY)

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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${TCLAP_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS}
                    ${EIGEN_INCLUDE_DIRS})

# enable CUDA support
if(MOD_CUDA)
    set(CUDA_VERBOSE_BUILD ON)
    find_package(CUDA 11.0 REQUIRED)
    set(CUDA_SEPARABLE_COMPILATION ON)
    set(CUDA_PROPAGATE_HOST_FLAGS OFF)
    set(CUDA_HOST_COMPILER g++)
    file(GLOB CUDA_FILES "src/" *.cu)
    cuda_select_nvcc_arch_flags(ARCH_FLAGS Common)
    message("Using: ${ARCH_FLAGS}")
    list(APPEND CUDA_NVCC_FLAGS ${ARCH_FLAGS})
    cuda_add_library(cuda_files ${CUDA_FILES})

    # create cuda test executable for probing cards
    add_executable(bramblecuda ${CMAKE_CURRENT_SOURCE_DIR}/bramblecuda.cpp)
    target_link_libraries(bramblecuda cuda_files)

    # set compile directive
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMOD_CUDA")
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

# set C++17 standard
add_definitions(-std=c++17 -march=native)

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
