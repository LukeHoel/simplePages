all: build link
withEngine: buildEngine build link
link:
	@g++  simplePages.o  olcPixelGameEngine/olcPixelGameEngine.o -pthread -lpng -lX11 -lGL
build:
	@g++ -c simpleDisplayer.cpp -o simplePages.o
buildEngine:
	@g++ -c olcPixelGameEngine/olcPixelGameEngine.cpp -o olcPixelGameEngine/olcPixelGameEngine.o