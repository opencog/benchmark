#
# Master Opencog CMake file.
#
# Need to have cmake-2.8.8 in order to be able to check the compiler
# version.
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)

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

# ----------------------------------------------------------
# Google benchmark package (optional)
FIND_PACKAGE(benchmark 1.3.0)
IF (benchmark_FOUND)
	MESSAGE(STATUS "Google Benchmark ${benchmark_VERSION} found.")
ELSE (benchmark_FOUND)
	MESSAGE(STATUS "Google Benchmark not found. Hint: see the README!")
ENDIF(benchmark_FOUND)

# ----------------------------------------------------------
# Check for boost.

SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREADED ON)

# Only basic boost (for tuple) is required
FIND_PACKAGE(Boost 1.46 REQUIRED)

IF(NOT Boost_FOUND)
	MESSAGE(FATAL_ERROR "Boost 1.46 or newer is needed to build benchmarks!")
ENDIF(NOT Boost_FOUND)

# ----------------------------------------------------------
# This is required for Guile
FIND_LIBRARY(GMP_LIBRARY gmp)
FIND_PATH(GMP_INCLUDE_DIR gmp.h)

# Gnu Guile scheme interpreter
# Version 2.2.2 is already specified by the atomspace.
FIND_PACKAGE(Guile 2.2.2.)
IF (GUILE_FOUND AND GMP_LIBRARY AND GMP_INCLUDE_DIR)
	ADD_DEFINITIONS(-DHAVE_GUILE)
	SET(HAVE_GUILE 1)
	INCLUDE_DIRECTORIES(${GUILE_INCLUDE_DIR})
ELSE (GUILE_FOUND AND GMP_LIBRARY AND GMP_INCLUDE_DIR)
	SET(GUILE_DIR_MESSAGE "Guile was not found; needed for guile benchmarks.")
	MESSAGE(STATUS "${GUILE_DIR_MESSAGE}")
ENDIF (GUILE_FOUND AND GMP_LIBRARY AND GMP_INCLUDE_DIR)

# ----------------------------------------------------------
# Python

FIND_PACKAGE(Python3Interp)
IF (3.4.0 VERSION_LESS ${PYTHON3_VERSION_STRING})
	SET (HAVE_PY_INTERP 1)
	SET (PYTHON_VER python3.5)
	MESSAGE(STATUS "Python ${PYTHON3_VERSION_STRING} interpreter found.")
ENDIF()

FIND_PACKAGE(PythonLibs)
IF (PYTHONLIBS_FOUND AND
     ((PYTHON3INTERP_FOUND AND 3.4.0 VERSION_LESS ${PYTHONLIBS_VERSION_STRING})
     OR
     (2.7.0 VERSION_LESS ${PYTHONLIBS_VERSION_STRING})))
	SET (HAVE_PY_LIBS 1)
	MESSAGE(STATUS "Python ${PYTHONLIBS_VERSION_STRING} libraries found.")
ELSE()
	MESSAGE(STATUS "Python libraries NOT found.")
ENDIF()

# ===================================================================
# Add subdirectories

ADD_SUBDIRECTORY(atomspace)
ADD_SUBDIRECTORY(micro)
ADD_SUBDIRECTORY(query-trite)

# ===================================================================
# Show a summary of what we got

SUMMARY_ADD("AtomSpace bench" "Synthetic benchmarks for AtomSpace methods" HAVE_ATOMSPACE)
SUMMARY_ADD("Python bench" "Synthetic benchmarks for Python (cython) bindings" HAVE_CYTHON)
SUMMARY_ADD("Scheme bench" "Synthetic benchmarks for Scheme (guile) bindings" HAVE_GUILE)
SUMMARY_ADD("Micro bench" "Assorted microbenchmarks" benchmark_FOUND)
SUMMARY_SHOW()
