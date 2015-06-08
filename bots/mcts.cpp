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
#include <cmath>

// timing stuff
#include <sys/times.h>
#include <unistd.h>

// #define MCTS_LOG_RATIOS stderr

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
			const Change& player_change = theGame.GetPlayerChange( theGame.GetCurrentPlayer() );
			const Change& pool = theGame.GetGameChange(); 

			Coin coins[COIN_COUNT];
			int cc=0;
			
			//for each coin that can be played
			for (const Coin *ci=&COINLIST[0];ci!=&COINLIST[COIN_COUNT];++ci)
				if (player_change.GetCount(*ci) > 0)
					coins[cc++] = *ci;
			
			//select one randomly
			Coin c = coins[rand()%cc];
			
			//get all possible sets of change for that coin
			GetAllPossibleChange(
				pool,
				c, 
				*mChangeList);

			int t = 0;
			for(int i=0;i!=mChangeList->size();++i)
			    t += (*mChangeList)[i].GetTotalValue();
			    
            int i = 0;
            if (t>0)
            {
                int r = rand()%t;
                for(;i!=mChangeList->size();++i)
                {
                    r -= (*mChangeList)[i].GetTotalValue();
                    if (r<=0) break;
                }
            }
            
			// select one randomly
			return Move(c, (*mChangeList)[i] );
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
			
			clock_t turnTime = timeLeft / (turnsLeft+1);
			
			std::vector<Move> moveList;
			GetAllMoves(theGame, moveList);
			int best = 0;
			
			if (moveList.size()>1)
			{
				// wins, simulations
				std::vector< std::pair<float, float> > wins_simulations;
				wins_simulations.resize( moveList.size(), std::pair<float, float>(0,1) );

				std::vector< float > uct;
				uct.resize( moveList.size(), 0 );

				const float c = sqrt(2);			
				int trials = 0;
				do
				{
					const float lnt = log(trials);
					int j=0;
					for (int i=0; i!=moveList.size(); ++i)
					{
						uct[i] = wins_simulations[i].first / wins_simulations[i].second;
						uct[i] += c * sqrt(lnt / wins_simulations[i].second);
						if (uct[i]>uct[j]) j = i;
					}
				
					GameState newGame = theGame.PlayMove( moveList[j] );
					wins_simulations[j].second++;
					if (PlayOut(newGame)==theGame.GetCurrentPlayer())
					{
						wins_simulations[j].first++;
						if ((wins_simulations[j].first/wins_simulations[j].second) > 
							(wins_simulations[best].first/wins_simulations[best].second))
							best = j;
					}
				
					trials++;
				
				}while( times(&t) - currentTurnClockStart < turnTime );
						
				#ifdef MCTS_LOG_RATIOS
				Serialise(MCTS_LOG_RATIOS, theGame);
				for (int i=0; i!=moveList.size(); ++i)
				{
					int a = wins_simulations[i].first / trials * 80;
					int b = wins_simulations[i].second / trials * 80;
					char txt[80];
					std::fill(txt, txt+a, '+');
					std::fill(txt+a, txt+b, '-');
					txt[b]=0;
					fprintf(MCTS_LOG_RATIOS, "%2i]%s%c\n", i, txt, i==best?'*':' ');
				}
				Serialise(MCTS_LOG_RATIOS, moveList[best]);
				#endif
				
			}
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
