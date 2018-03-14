objects = polar_encoder.o utilities.o channel.o polar.o polar_construction.o polar_decoder.o
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

polar_construction.o: polar_construction.cpp polar_construction.hpp
	g++ -g -c polar_construction.cpp

polar_decoder.o: polar_decoder.cpp polar_decoder.hpp utilities.hpp polar_encoder.hpp
	g++ -g -c polar_decoder.cpp

.PHONY: clean

clean:
	rm -f $(objects)
