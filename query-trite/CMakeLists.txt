# Build file for the atomspace pattern matching benchmarks

IF (HAVE_GUILE)
    ADD_EXECUTABLE (query_benchmark
        query_benchmark.cc
        )

    TARGET_LINK_LIBRARIES (query_benchmark
        ${ATOMSPACE_LIBRARIES}
        ${COGUTIL_LIBRARY}
        )

    ADD_CUSTOM_TARGET(run_query_benchmark
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/query_benchmark -c query_benchmark.conf
        )
ENDIF (HAVE_GUILE)
