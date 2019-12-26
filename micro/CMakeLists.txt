# Build file for the atomspace microbenchmarks

IF (BUILD_MICRO)
	ADD_EXECUTABLE (benchmark
		benchmark.cc
		scopelink_bm.cc
		evaluationlink_bm.cc
		variables_bm.cc
		forwardchainer_bm.cc
	)

	TARGET_LINK_LIBRARIES (benchmark
		${COGUTIL_LIBRARY}
		${ATOMSPACE_LIBRARIES}
		${URE_LIBRARY}
		benchmark::benchmark
	)
ENDIF(BUILD_MICRO)
