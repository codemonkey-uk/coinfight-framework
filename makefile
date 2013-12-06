CFLAGS=-I. -Wall
CXXFLAGS=-I. -Wall

obj = cppFIGHT.o cplayers.o cexample.o example.o pplayer.o

fight: $(obj)
	g++ -o fight $(obj)
	
clean:
	rm fight *.o