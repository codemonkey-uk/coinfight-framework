CFLAGS=-I. -Wall -O3 
CXXFLAGS=-I. -Wall -O3

obj = cppFIGHT.o cplayers.o cexample.o example.o pplayer.o argumentProcessor.o tournamentResultsFormatter.o
bots_obj = bots/thad.o bots/wookie.o bots/mcts.o

fight: $(obj)
	g++ -o fight $(obj)

fight_all: $(obj) $(bots_obj)
	g++ -o fight_all $(obj) $(bots_obj)

	
# dependancies
obj : cFIGHT.h
bots_obj : cppFIGHT.h
example.o : cppFIGHT.h
argumentProcessor.o : argumentProcessor.h
cppFIGHT.o pplayer.o : cppFIGHT.h pplayer.h argumentProcessor.h tournamentResultsFormatter.h
tournamentResultsFormatter.o : tournamentResultsFormatter.h

.PHONY : clean
clean:
	rm fight $(obj)
	rm fight_all $(bots_obj)