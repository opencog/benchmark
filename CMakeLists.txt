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
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/lib/")

# ===============================================================
# Detect different compilers and OS'es, tweak flags as necessary.

IF (CMAKE_COMPILER_IS_GNUCXX)
	# gcc before version 4.8 does not support the c++11 definition
	# of thread_local.
	IF (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8.0)
		MESSAGE(FATAL_ERROR "GCC version must be at least 4.8.0!")
	ENDIF (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8.0)

	IF (APPLE)
		CMAKE_POLICY(SET CMP0042 NEW)  # Something about MACOSX_RPATH

		SET(CMAKE_C_FLAGS "-Wall -Wno-long-long -Wno-conversion")
		SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")
		SET(CMAKE_C_FLAGS_PROFILE "-O0 -pg")
		SET(CMAKE_C_FLAGS_RELEASE "-O2 -g0")
		# Vital to do this otherwise unresolved symbols everywhere:
		SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-flat_namespace,-undefined,dynamic_lookup")
		SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-flat_namespace,-undefined,dynamic_lookup")

		# The Apple linker does not support the --no-as-needed flag.
		SET(NO_AS_NEEDED "")

	ELSE (APPLE)
		SET(CMAKE_C_FLAGS "-Wall -fPIC")
		# SET(CMAKE_C_FLAGS "-Wl,--copy-dt-needed-entries")
		SET(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb3 -fstack-protector")
		SET(CMAKE_C_FLAGS_PROFILE "-O2 -g3 -fstack-protector -pg")
		# -flto is good for performance, but wow is it slow to link...
		# XXX disable for now ... its just to painful, in daily life.
		# ... except that the buildbot needs this, to pass unit tests.
		# Current failure is in libatomcore, which depends on atomutils
		# (ldd -r libatomcore.so shows unresolved symbols.)
		SET(CMAKE_C_FLAGS_RELEASE "-O3 -g -fstack-protector")
		# SET(CMAKE_C_FLAGS_RELEASE "-O3 -g -fstack-protector -flto")
		# SET(CMAKE_C_FLAGS_RELEASE "-O3 -g -fstack-protector -flto=8")

		SET(NO_AS_NEEDED "-Wl,--no-as-needed")
		LINK_LIBRARIES(pthread)

		# Workaround for circular dependencies problem. This causes
		# LINK_LIBRARIES to appear twice in the linker line; the
		# second appearance has the effect of resolving any circular
		# dependencies left over from the first specification.
		# See pull request #642 for more discussion.
		SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} <LINK_LIBRARIES>")
	ENDIF (APPLE)

	# 1) -Wno-variadic-macros is to avoid warnings regarding using
	# variadic in macro OC_ASSERT (the warning warns that this is only
	# available from C99, lol!)
	#
	# 2) -fopenmp for multithreading support
	#
	# 3) -std=gnu++11 for C++11 and GNU extensions support
	SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wno-variadic-macros -fopenmp -std=gnu++11")

	SET(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
	SET(CMAKE_CXX_FLAGS_PROFILE ${CMAKE_C_FLAGS_PROFILE})
	SET(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})

	# Options for generating gcov code coverage output
	SET(CMAKE_C_FLAGS_COVERAGE "-O0 -g -fprofile-arcs -ftest-coverage -fno-inline")
	SET(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_C_FLAGS_COVERAGE} -fno-default-inline")
	# Might be needed for some combinations of ln and gcc
	IF (CMAKE_BUILD_TYPE STREQUAL "Coverage")
		LINK_LIBRARIES(gcov)
	ENDIF (CMAKE_BUILD_TYPE STREQUAL "Coverage")
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

# ===================================================================
# Check for existance of various required, optional packages.
# Check for boost. We need dynamic-linked, threaded libs by default.

SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREADED ON)

# Required boost packages
# hypertable requires boost iostreams
# boost-1.49 no longer has a libboost_iostreams
# 1.46 is minimum for required filesystem support
# program_options needed by some combo utilities
FIND_PACKAGE(Boost 1.46 COMPONENTS date_time filesystem program_options regex serialization system thread REQUIRED)

IF(Boost_FOUND)
	SET(Boost_FOUND_SAVE 1)

	# Boost.Signals2 requires this in signal_type.hpp
	SET(BOOST_PARAMETER_MAX_ARITY 7)
	ADD_DEFINITIONS(-DBOOST_PARAMETER_MAX_ARITY=${BOOST_PARAMETER_MAX_ARITY})
ELSE(Boost_FOUND)
	MESSAGE(FATAL_ERROR "Boost 1.46 or newer is needed to build OpenCog!")
ENDIF(Boost_FOUND)

# Opencog won't compile with Boost 1.51, some kind of conflict with
# hash functions, see github bugs 1 and 36
IF(105100 EQUAL ${Boost_VERSION})
	MESSAGE(FATAL_ERROR "Boost version 1.51 will not work with OpenCog.  Please use a different version.")
ENDIF(105100 EQUAL ${Boost_VERSION})
MESSAGE(STATUS "Boost version ${Boost_VERSION} found.")

# Optional boost packages; can build without these.
# FIND_PACKAGE(Boost 1.46 COMPONENTS python program_options QUIET)
# FIND_PACKAGE(Boost 1.46 COMPONENTS math_c99 QUIET)

# Arghhh. Except cmake is treating above as required, not optional. #$%**&
IF(Boost_FOUND_SAVE)
	SET(Boost_FOUND 1)
ENDIF(Boost_FOUND_SAVE)

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

# This is required for Guile
FIND_LIBRARY(GMP_LIBRARY gmp)
FIND_PATH(GMP_INCLUDE_DIR gmp.h)

# Gnu Guile scheme interpreter
# Version 2.0.9 is needed for mrs io ports, compilation, etc.
FIND_PACKAGE(Guile 2.0.9)
IF (GUILE_FOUND AND GMP_LIBRARY AND GMP_INCLUDE_DIR)
	ADD_DEFINITIONS(-DHAVE_GUILE)
	SET(HAVE_GUILE 1)
	INCLUDE_DIRECTORIES(${GUILE_INCLUDE_DIR})
ELSE (GUILE_FOUND AND GMP_LIBRARY AND GMP_INCLUDE_DIR)
	SET(GUILE_DIR_MESSAGE "Guile was not found; the scheme shell will not be built.\nTo over-ride, make sure GUILE_LIBRARIES and GUILE_INCLUDE_DIRS are set.")
	MESSAGE(STATUS "${GUILE_DIR_MESSAGE}")
ENDIF (GUILE_FOUND AND GMP_LIBRARY AND GMP_INCLUDE_DIR)

# -----------------------------------------------------------------------------
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
ADD_SUBDIRECTORY(query)

# ===================================================================
# Show a summary of what we got

include(lib/Summary.cmake)
SUMMARY_ADD("AtomSpace benchmarks" "Microbenchmarks for atomspace classes and methods" HAVE_ATOMSPACE)
SUMMARY_ADD("Python bindings benchmarks" "Microbenchmarks using Python (cython) bindings" HAVE_CYTHON)
SUMMARY_ADD("Scheme benchmarks" "Benchmarks on data described in Scheme language" HAVE_GUILE)
SUMMARY_SHOW()

