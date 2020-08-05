# Build file for the atomspace microbenchmarks

LIST(APPEND LIST_MODULES
	benchmark.cc
	pointercast_bm.cc
	atomspace_bm.cc
	addnode_bm.cc
	addlink_bm.cc
	evaluationlink_bm.cc
	large_simple_bm.cc
	large_flat_bm.cc
	large_zipf_bm.cc
	values_bm.cc
	scopelink_bm.cc
	variables_bm.cc
	)

IF (HAVE_URE)
	LIST(APPEND LIST_MODULES forwardchainer_bm.cc)
	LIST(APPEND LIST_LIBRARIES ${URE_LIBRARY})
ENDIF(HAVE_URE)

LIST(APPEND LIST_LIBRARIES
	${ATOMSPACE_LIBRARIES}
	${COGUTIL_LIBRARY}
	benchmark::benchmark
	)

ADD_EXECUTABLE (benchmark "${LIST_MODULES}")
TARGET_LINK_LIBRARIES (benchmark "${LIST_LIBRARIES}")
