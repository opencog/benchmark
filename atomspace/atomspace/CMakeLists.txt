# Build file for the atomspace microbenchmarks

ADD_EXECUTABLE (atomspace_bm
	AtomSpaceBenchmark.cc
	atomspace_bm.cc
)

TARGET_LINK_LIBRARIES (atomspace_bm
	${ATOMSPACE_LIBRARIES}
	${COGUTIL_LIBRARY}
)

IF (benchmark_FOUND)
	ADD_EXECUTABLE (benchmark
		benchmark.cc
		scopelink_bm.cc
		evaluationlink_bm.cc
	)

	TARGET_LINK_LIBRARIES (benchmark
		${ATOMSPACE_LIBRARIES}
		${COGUTIL_LIBRARY}
		benchmark::benchmark
		benchmark::benchmark_main
	)
ENDIF(benchmark_FOUND)

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
