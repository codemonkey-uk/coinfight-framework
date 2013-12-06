CFLAGS=-I. -Wall
CXXFLAGS=-I. -Wall

obj = cppFIGHT.o cplayers.o cexample.o example.o pplayer.o

fight: $(obj)
	g++ -o fight $(obj)
	
# dependancies
obj : cFIGHT.h
example.o : cppFIGHT.h
cppFIGHT.o pplayer.o : cppFIGHT.h pplayer.h

.PHONY : clean
clean:
	rm fight $(obj)