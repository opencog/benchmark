#
# Master Opencog CMake file.
#
# General organization:
# -- check for different compilers, OS'es
# -- search for various required & optional libraries/tools
# -- decide what to build based on above results.
# -- configure various config files.
# -- print pretty summary
#

# Need to have cmake-2.8.8 in order to be able to check the compiler
# version.
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)

PROJECT(benchmark)

# add the 'lib' dir to cmake's module search path
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/lib/")

# Cogutil
FIND_PACKAGE(CogUtil)
IF (COGUTIL_FOUND)
        MESSAGE(STATUS "CogUtil found.")
        ADD_DEFINITIONS(-DHAVE_COGUTIL)
        SET(HAVE_COGUTIL 1)
        INCLUDE_DIRECTORIES(${COGUTIL_INCLUDE_DIR})
ELSE (COGUTIL_FOUND)
        MESSAGE(FATAL_ERROR "CogUtil missing: it is needed for everything!")
ENDIF (COGUTIL_FOUND)

# AtomSpace
FIND_PACKAGE(AtomSpace 5.0.3 REQUIRED)
IF (ATOMSPACE_FOUND)
        MESSAGE(STATUS "AtomSpace found.")
        ADD_DEFINITIONS(-DHAVE_ATOMSPACE)
        SET(HAVE_ATOMSPACE 1)
        INCLUDE_DIRECTORIES(${ATOMSPACE_INCLUDE_DIR})
ELSE (ATOMSPACE_FOUND)
        MESSAGE(FATAL_ERROR "AtomSpace missing: it is needed for everything!")
ENDIF (ATOMSPACE_FOUND)

ADD_SUBDIRECTORY(atomspace)

