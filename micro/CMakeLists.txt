# Build file for the atomspace microbenchmarks

LIST(APPEND LIST_MODULES
	benchmark.cc
	scopelink_bm.cc
	evaluationlink_bm.cc
	variables_bm.cc
	)

LIST(APPEND LIST_LIBRARIES
	${COGUTIL_LIBRARY}
	${ATOMSPACE_LIBRARIES}
	benchmark::benchmark
	)

IF (HAVE_URE)
	LIST(APPEND LIST_MODULES forwardchainer_bm.cc)
	LIST(APPEND LIST_LIBRARIES ${URE_LIBRARY})
ENDIF(HAVE_URE)

ADD_EXECUTABLE (benchmark "${LIST_MODULES}")
TARGET_LINK_LIBRARIES (benchmark "${LIST_LIBRARIES}")
