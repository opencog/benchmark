# Build file for the atomspace pattern matching benchmarks

IF (HAVE_GUILE)
        ADD_EXECUTABLE (bindlink_bm
                bindlink_bm.cc
        )

        TARGET_LINK_LIBRARIES (bindlink_bm
                ${ATOMSPACE_LIBRARIES}
                ${COGUTIL_LIBRARY}
        )
ENDIF (HAVE_GUILE)
