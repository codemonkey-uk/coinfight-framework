#include "cppFIGHT.h"

#ifndef CPPFIGHT_PPLAYER_H_INCLUDED
#define CPPFIGHT_PPLAYER_H_INCLUDED

class POpenPlayer : public CPPFight::Player
{
	public:

		//pass your entry's title, and your name
		POpenPlayer( const std::string& commandline,
			const std::string& title, const std::string& author );
		
		// the main hoo-har, examine the game, and return your move...
		CPPFight::Move GetMove( const CPPFight::Game& theGame );

		// optional overloads
		// notifies player of game status, start, win/loose
		// useful for learning AI's
		//virtual void NotifyGameStart(const Game& theGame);
		//virtual void NotifyWon();
		//virtual void NotifyEliminated();
		
		//dtor, you know the score.
		// virtual ~Player();

	private:
		std::string mCommand;
		int myInFp;
		int myOutFp;
		pid_t myPid;
		FILE* m_pIn;
		FILE* m_pOut;
};

#endif