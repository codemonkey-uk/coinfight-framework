CFLAGS=-I. -Wall
CXXFLAGS=-I. -Wall

obj = cppFIGHT.o cplayers.o cexample.o example.o pplayer.o argumentProcessor.o tournamentResultsFormatter.o

fight: $(obj)
	g++ -o fight $(obj)
	
# dependancies
obj : cFIGHT.h
example.o : cppFIGHT.h
argumentProcessor.o : argumentProcessor.h
cppFIGHT.o pplayer.o : cppFIGHT.h pplayer.h argumentProcessor.h tournamentResultsFormatter.h
tournamentResultsFormatter.o : tournamentResultsFormatter.h

.PHONY : clean
clean:
	rm fight $(obj)