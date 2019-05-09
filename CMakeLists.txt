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

# uncomment to be in Release mode [default]
# SET(CMAKE_BUILD_TYPE Release)

# uncomment to build in debug mode
# SET(CMAKE_BUILD_TYPE Debug)

# uncomment to be in coverage testing mode
# SET(CMAKE_BUILD_TYPE Coverage)

# uncomment to build in profile mode
# SET(CMAKE_BUILD_TYPE Profile)

# uncomment to build in release mode with debug information
# SET(CMAKE_BUILD_TYPE RelWithDebInfo)

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
FIND_PACKAGE(benchmark)
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
# Python and Cython
#
# NOTE: Python interpreter is needed for runing python unit tests, and for
# running the FindCython module.
#
# Search for Python3 first, and use that, if found. Else use Python2.

FIND_PACKAGE(Python3Interp)
IF (3.4.0 VERSION_LESS ${PYTHON3_VERSION_STRING})
	SET (HAVE_PY_INTERP 1)
	SET (PYTHON_VER python3.5)
	MESSAGE(STATUS "Python ${PYTHON3_VERSION_STRING} interpreter found.")
ENDIF()

IF (NOT HAVE_PY_INTERP)
	FIND_PACKAGE(PythonInterp)
	IF (2.7.0 VERSION_LESS ${PYTHON_VERSION_STRING})
		SET (HAVE_PY_INTERP 1)
		SET (PYTHON_VER python2.7)
		MESSAGE(STATUS "Python ${PYTHON_VERSION_STRING} interpreter found.")
	ENDIF()
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

# Cython is used to generate python bindings.
IF(HAVE_PY_INTERP)
	FIND_PACKAGE(Cython 0.19.0)

	IF (CYTHON_FOUND AND HAVE_PY_LIBS)
		# Find python destination dir for python bindings because it may
		# differ on each operating system.
		ADD_DEFINITIONS(-DHAVE_CYTHON)
		SET(HAVE_CYTHON 1)

		# Hack together some reasonable install location.
		SET(PYTHON_DEST
			"lib${LIB_DIR_SUFFIX}/${PYTHON_VER}/dist-packages/opencog")
		MESSAGE(STATUS
			"Python install dir: ${CMAKE_INSTALL_PREFIX}/${PYTHON_DEST}" )

	ELSE (CYTHON_FOUND AND HAVE_PY_LIBS)
		IF(NOT CYTHON_FOUND)
			MESSAGE(STATUS "Cython executable not found.")
		ENDIF(NOT CYTHON_FOUND)
	ENDIF (CYTHON_FOUND AND HAVE_PY_LIBS)

	# Nosetests will find and automatically run python tests.
	IF (PYTHON3INTERP_FOUND)
		FIND_PROGRAM(NOSETESTS_EXECUTABLE nosetests3)
	ELSE ()
		FIND_PROGRAM(NOSETESTS_EXECUTABLE nosetests)
	ENDIF ()
	IF (NOSETESTS_EXECUTABLE AND CYTHON_FOUND AND HAVE_PY_LIBS)
		SET(HAVE_NOSETESTS 1)
		MESSAGE(STATUS "Using nosetests executable " ${NOSETESTS_EXECUTABLE})
	ENDIF (NOSETESTS_EXECUTABLE AND CYTHON_FOUND AND HAVE_PY_LIBS)
ENDIF(HAVE_PY_INTERP)

# ===================================================================
# Add subdirectories

ADD_SUBDIRECTORY(atomspace)

# ===================================================================
# Show a summary of what we got

SUMMARY_ADD("AtomSpace bench" "Microbenchmarks for atomspace classes and methods" HAVE_ATOMSPACE)
SUMMARY_ADD("Python bench" "Microbenchmarks for Python (cython) bindings" HAVE_CYTHON)
SUMMARY_ADD("Scheme bench" "Benchmarks for Scheme (guile) bindings" HAVE_GUILE)
SUMMARY_SHOW()
