# Build file for the atomspace benchmark

ADD_EXECUTABLE (atomspace_bm
        AtomSpaceBenchmark.cc
        atomspace_bm.cc
)

TARGET_LINK_LIBRARIES (atomspace_bm
        ${ATOMSPACE_LIBRARIES}
        ${COGUTIL_LIBRARY}
)

IF (HAVE_CYTHON)
        INCLUDE_DIRECTORIES (
                ${PYTHON_INCLUDE_DIRS}
        )
        TARGET_LINK_LIBRARIES (atomspace_bm
                atomspace_cython
                ${PYTHON_LIBRARIES}
        )
ENDIF(HAVE_CYTHON)

IF (HAVE_GUILE)
        ADD_EXECUTABLE (profile_bindlink
                profile_bindlink.cc
        )

        TARGET_LINK_LIBRARIES (profile_bindlink
                ${ATOMSPACE_LIBRARIES}
                ${COGUTIL_LIBRARY}
        )
ENDIF (HAVE_GUILE)
