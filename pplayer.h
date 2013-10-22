#include "cppFIGHT.h"

#ifndef CPPFIGHT_PPLAYER_H_INCLUDED
#define CPPFIGHT_PPLAYER_H_INCLUDED

class POpenPlayer : public CPPFight::Player
{
	public:

		//pass your entry's title, and your name
		POpenPlayer( 
			const std::vector<std::string>& commands,
			const std::string& title, 
			const std::string& author );
		
		~POpenPlayer();
		
		// the main hoo-har, examine the game, and return your move...
		CPPFight::Move GetMove( const CPPFight::Game& theGame );

		// optional overloads
		// notifies player of game status, start, win/loose
		// useful for learning AI's
		//virtual void NotifyGameStart(const Game& theGame);
		//virtual void NotifyWon();
		//virtual void NotifyEliminated();

	private:
		char** mCommands;
};

#endif