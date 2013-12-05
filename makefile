CFLAGS=-I. -Wall
CXXFLAGS=-I. -Wall

fight: cppFIGHT.o cplayers.o cexample.o example.o pplayer.o
	g++ -o fight cppFIGHT.o cplayers.o cexample.o example.o pplayer.o
	
clean:
	rm fight *.o