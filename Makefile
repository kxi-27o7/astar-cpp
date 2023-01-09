output: main.o vec2.o
	g++ main.o vec2.o -o output

main.o: main.cpp
	g++ -c main.cpp

vec2.o: vec2.cpp vec2.hpp
	g++ -c vec2.cpp

clean:
	rm *.o output
