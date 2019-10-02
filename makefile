all: buildApp link
withEngine: buildEngine buildApp link
link:
	@g++  build/simplePages.o  build/olcPixelGameEngine.o -pthread -lpng -lX11 -lGL
buildApp:
	@g++ -c src/display.cpp -o build/simplePages.o
buildEngine:
	@g++ -c olcPixelGameEngine/olcPixelGameEngine.cpp -o build/olcPixelGameEngine.o