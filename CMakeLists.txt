#
# Master Opencog CMake file.
#
CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
CMAKE_POLICY(SET CMP0028 NEW)

PROJECT(benchmark)

# default build type
IF (CMAKE_BUILD_TYPE STREQUAL "")
	SET(CMAKE_BUILD_TYPE Release)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "")

MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# add the 'lib' dir to cmake's module search path
# SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/lib/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/lib/")

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

# add the 'cmake' directory from cogutil to search path
list(APPEND CMAKE_MODULE_PATH  ${COGUTIL_DATA_DIR}/cmake)

include(${COGUTIL_DATA_DIR}/cmake/OpenCogGccOptions.cmake)
include(${COGUTIL_DATA_DIR}/cmake/OpenCogLibOptions.cmake)
include(${COGUTIL_DATA_DIR}/cmake/OpenCogInstallOptions.cmake)
include(${COGUTIL_DATA_DIR}/cmake/Summary.cmake)

# ===================================================================
# Check for existance of various required, optional packages.

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

# URE (optional)
FIND_PACKAGE(URE 1.0.0 CONFIG)
IF (URE_FOUND)
	MESSAGE(STATUS "URE found.")
	SET(HAVE_URE 1)
ELSE (URE_FOUND)
	MESSAGE(STATUS "URE was not found. URE micro benchmarks will not be built.")
ENDIF (URE_FOUND)

# ----------------------------------------------------------
# Google benchmark package (optional)
FIND_PACKAGE(benchmark 1.3.0)
IF (benchmark_FOUND)
	SET(BUILD_MICRO 1)
	MESSAGE(STATUS "Google Benchmark ${benchmark_VERSION} found.")
ELSE (benchmark_FOUND)
	MESSAGE(STATUS "Google Benchmark not found. Micro benchmarks will not be built.")
ENDIF(benchmark_FOUND)

# ----------------------------------------------------------
# Check for boost.

SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREADED ON)

# Only basic boost (for tuple) is required
FIND_PACKAGE(Boost 1.46 REQUIRED)

# *We* don't actually need Boost::system but some build fail with
# hard-to-debug errors: `/usr/bin/ld: cannot find -lBoost::system`
# because I guess the AtomSpace does need this!? So cave on the issue
# and just engage the foolishness.
FIND_PACKAGE(Boost ${MIN_BOOST} REQUIRED COMPONENTS system)

IF(NOT Boost_FOUND)
	MESSAGE(FATAL_ERROR "Boost 1.46 or newer is needed to build benchmarks!")
ENDIF(NOT Boost_FOUND)

# ----------------------------------------------------------
# This is required for Guile, python

include(OpenCogFindGuile)
include(OpenCogFindPython)

# ===================================================================
# Add subdirectories

ADD_SUBDIRECTORY(atomspace)
IF (BUILD_MICRO)
	ADD_SUBDIRECTORY(micro)
ENDIF(BUILD_MICRO)
ADD_SUBDIRECTORY(query-trite)

# ===================================================================
# Show a summary of what we got

SUMMARY_ADD("AtomSpace bench" "Synthetic benchmarks for AtomSpace methods" HAVE_ATOMSPACE)
SUMMARY_ADD("Python bench" "Synthetic benchmarks for Python (cython) bindings" HAVE_CYTHON)
SUMMARY_ADD("Scheme bench" "Synthetic benchmarks for Scheme (guile) bindings" HAVE_GUILE)
SUMMARY_ADD("Micro bench" "Assorted microbenchmarks" BUILD_MICRO)
SUMMARY_ADD("URE Micro bench" "URE microbenchmarks" BUILD_MICRO AND HAVE_URE)
SUMMARY_SHOW()
