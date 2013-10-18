#Required version of cmake
cmake_minimum_required (VERSION 2.8)

#Force a decent compiler on OSX (gcc is still 4.2 on OSX, and it lacks functionality we need)
#  If you have an "/Applications" directory on *nix... just why?
#  If more people start using this, I'll clean up the search order, I promise. ~Seth
IF(EXISTS "/Applications/")
  #Gcc 4.X?
  IF(EXISTS "/opt/local/bin/g++-mp-4.6")
    SET(CMAKE_C_COMPILER    "/opt/local/bin/gcc-mp-4.6")
    SET(CMAKE_CXX_COMPILER  "/opt/local/bin/g++-mp-4.6")
  ENDIF(EXISTS "/opt/local/bin/g++-mp-4.6")
  
  #Clang? (built from SVN, will override gcc)
  #IF(EXISTS "/usr/local/bin/clang")
  #  SET(CMAKE_C_COMPILER    "/usr/local/bin/clang")
  #  SET(CMAKE_CXX_COMPILER  "/usr/local/bin/clang++")
  #ENDIF(EXISTS "/usr/local/bin/clang")
ENDIF(EXISTS "/Applications/")

#Project name. Used to tag resources in cmake. 
project (Portentia)

#Default build type is Debug
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type was specified; setting to Debug by default.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#Additional module find scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")

#Option: build tests. Currently has no effect.
option(BUILD_TESTS "Build unit tests." OFF)

#Turn on verbose output
SET(CMAKE_VERBOSE_MAKEFILE ON)

#Force gcc to output single line errors. 
# This makes it easier for Eclipse to parse and understand each error.
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#Just assume we are using a compiler that supports C++0x; 
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

#Include our top-level source directory, for easy locating.
include_directories("${PROJECT_SOURCE_DIR}/src")

#List of libraries to link
SET(LibraryList "")

#Find Boost
set(Boost_ADDITIONAL_VERSIONS "1.47" "1.47.0" "1.48" "1.48.0" "1.49" "1.49.0" "1.50" "1.50.0")
find_package(Boost REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIR})
LIST(APPEND LibraryList ${Boost_LIBRARIES})

#Find SFML
find_package(SFML REQUIRED)
include_directories(${SFML_INCLUDE_DIR})
LIST(APPEND LibraryList ${SFML_LIBRARY})

#Find the "spatial index" library. Ubuntu has it, or: (https://github.com/libspatialindex/libspatialindex)
find_package(SpatialIndex)
include_directories(${SPATIALINDEX_INCLUDE_DIR})
LIST(APPEND LibraryList ${SPATIALINDEX_LIBRARY})

#Our source files.
FILE(GLOB_RECURSE OurSrcFiles "src/*.c" "src/*.cpp")

#Create our executable.
ADD_EXECUTABLE(Portentia ${OurSrcFiles})
TARGET_LINK_LIBRARIES(Portentia  ${LibraryList})

