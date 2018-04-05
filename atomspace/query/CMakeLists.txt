# Build file for the atomspace pattern matching benchmarks

IF (HAVE_GUILE)
        ADD_EXECUTABLE (benchmark
                benchmark.cc
        )

        TARGET_LINK_LIBRARIES (benchmark
                ${ATOMSPACE_LIBRARIES}
                ${COGUTIL_LIBRARY}
        )
ENDIF (HAVE_GUILE)
