run:
	cmake -H. -Bbuild
	cmake --build build
	./build/Chess

test:
	g++ -c ./src/main.cpp
	g++ main.o -o ./build/Chess -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
	./build/Chess
