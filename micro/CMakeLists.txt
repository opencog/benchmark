# Build file for the atomspace microbenchmarks

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
	)
ENDIF(benchmark_FOUND)
