# Build file for the atomspace synthetic benchmarks

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
		${Boost_SYSTEM_LIBRARY}
		${PYTHON_LIBRARIES}
		${ATOMSPACE_PythonEval_LIBRARY}
	)
ENDIF(HAVE_CYTHON)
