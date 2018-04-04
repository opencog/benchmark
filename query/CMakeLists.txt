# Build file for the atomspace pattern matching benchmarks

IF (HAVE_GUILE)
        ADD_EXECUTABLE (profile_bindlink
                profile_bindlink.cc
        )

        TARGET_LINK_LIBRARIES (profile_bindlink
                ${ATOMSPACE_LIBRARIES}
                ${COGUTIL_LIBRARY}
        )
ENDIF (HAVE_GUILE)
