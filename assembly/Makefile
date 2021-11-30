

all: pcast pcast.dump pcast-O3.dump pcast-O3.bil


pcast-O3.o: pointercast.cc
	g++ -std=c++17 -O3 -c pointercast.cc
	mv pointercast.o pcast-O3.o

pcast-O3.dump: pcast-O3.o
	objdump -d -C pcast-O3.o > pcast-O3.dump

# Binary analysis platform
pcast-O3.bil: pcast-O3.o
	bap pcast-O3.o -d > pcast-O3.bil

pcast.dump: pointercast.cc
	g++ -std=c++17 -c pointercast.cc
	mv pointercast.o pcast.o
	objdump -d -C pcast.o > pcast.dump

pcast: pointercast.cc
	g++ -std=c++17 -O3 -o pcast pointercast.cc \
		-L/usr/local/lib/opencog -latom_types -latombase -latomspace -lvalue
