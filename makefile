objects = polar_encoder.o utilities.o channel.o polar.o
default: $(objects)
	g++ -g -o polar.out $(objects)

polar.o: polar.cpp polar_encoder.hpp utilities.hpp channel.hpp
	g++ -g -c polar.cpp

polar_encoder.o: polar_encoder.cpp polar_encoder.hpp utilities.hpp
	g++ -g -c polar_encoder.cpp

utilities.o: utilities.hpp utilities.cpp
	g++ -g -c utilities.cpp

channel.o: channel.cpp channel.hpp
	g++ -g -c channel.cpp

clean:
	rm -f $(objects)
