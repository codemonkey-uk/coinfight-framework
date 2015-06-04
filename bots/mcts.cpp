//
// "MCTS"
//

#include "cppFIGHT.h"

#include <limits>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <cstdlib>

// timing stuff
#include <sys/times.h>
#include <unistd.h>

namespace Thad{

	using namespace CPPFight;

	//
	// Monte Carlo tree search FIGHT Player
	//
	class MCTSBot : public Player
	{
	public:

		//construct base class with AI name and author name
		MCTSBot()
		 : Player( "MCTS", "Thad" )
		 , mChangeList( new Change::List )
		{
		}

		void GetAllMoves( const GameState& theGame, std::vector<Move>& moveListOut )
		{
			const Change& player_change = theGame.GetPlayerChange( theGame.GetCurrentPlayer() );
			const Change& pool = theGame.GetGameChange(); 

			// need to tell resize to will with a value even when count is 0
			moveListOut.resize(0, Move(PENNY));
			
			//for each coin that can be played
			for (const Coin *ci=&COINLIST[0];ci!=&COINLIST[COIN_COUNT];++ci){				
				if (player_change.GetCount(*ci) > 0){
					
					//get all possible sets of change into array
					GetAllPossibleChange(
						pool,
						*ci, 
						*mChangeList);

					//for each possible set of change
					const Change::List::iterator e = mChangeList->end();
					for(Change::List::iterator i = mChangeList->begin();i!=e;++i){
				
						//create "move" from coin to give, and change to take				
						const Move m(*ci,*i);
						moveListOut.push_back(m);
					}
				}
			}
		}
		
		Move GetRandomMove( const GameState& theGame )
		{
			static std::vector<Move> moveList;
			
			GetAllMoves(theGame, moveList);
			
			// random move
			return moveList[rand()%moveList.size()];
		}
		
		int PlayOut(GameState theGame)
		{
			while (theGame.GetActivePlayers()>1)
				theGame = theGame.PlayMove( GetRandomMove(theGame) );
			return theGame.GetCurrentPlayer();
		}

		//override GetMove function
		virtual Move GetMove (const Game& theGame)
		{
			tms t;
			clock_t currentTurnClockStart = times(&t);
			
			clock_t timeLeft = CFIGHT_PLAYER_TIME_PER_GAME - mTimeTaken;

			// we could get this from the PlayOut, but this is worst (best) case
			int turnsLeft = theGame.GetPlayerChange( 
				theGame.GetCurrentPlayer() 
			).GetTotalValue();
			
			clock_t turnTime = timeLeft / turnsLeft;
			
			std::vector<Move> moveList;
			GetAllMoves(theGame, moveList);
			std::vector<int> scores;
			scores.resize( moveList.size(), 0 );

			int best = 0;
			int trials = 0;
			do
			{
				for (int i=0; i!=moveList.size(); ++i)
				{
					GameState newGame = theGame.PlayMove( moveList[i] );
					if (PlayOut(newGame)==theGame.GetCurrentPlayer())
					{
						scores[i]++;
						if (scores[i] > scores[best])
							best = i;
					}
				}
				trials++;				
			}while( times(&t) - currentTurnClockStart < turnTime );
			mTimeTaken += times(&t) - currentTurnClockStart;
			return moveList[best];
		}

		void NotifyGameStart(const Game& theGame)
		{
			mTimeTaken = 0;
		}

		clock_t mTimeTaken;
		Change::List* mChangeList;

	// create an instance of this player
	} mctsPlayer;
};
