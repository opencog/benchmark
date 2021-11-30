

all: pointercast.dump

pointercast.o: pointercast.cc
	g++ -std=c++17 -c pointercast.cc
	# g++ -std=c++17 -O3 -c pointercast.cc

pointercast.dump: pointercast.o
	objdump -d pointercast.o > pointercast.dump
