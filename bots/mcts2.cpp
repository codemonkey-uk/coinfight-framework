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
	
    const float uct_c = sqrt(2);
    
	//
	// Monte Carlo tree search FIGHT Player
	//
	class MCTSBot2 : public Player
	{
	public:

		//construct base class with AI name and author name
		MCTSBot2()
		 : Player( "MCTS2", "Thad" )
		 , mChangeList( new Change::List )
		{
		}
		
		struct Node
		{
		    Node( const Move& move )
		        : mMove(move)
		        , mWins(0)
		        , mSims(1)
		    { }
		    
		    Move mMove;
		    int mWins;
		    int mSims;
		    
		    float UCT(float lnt) const
		    {
		        float s = std::max(1.0f,(float)mSims);
		        float uct = mWins / s;
		        uct += uct_c * sqrt(lnt / s);
		        return uct;
		    }
		    
		    float Ratio() const
		    {
		        return (float)mWins / (float)mSims;
		    }
		    
		    std::vector<Node>* mChildren;
		};

		std::vector<Node>* GetAllMoves( const GameState& theGame )
		{
			const Change& player_change = theGame.GetPlayerChange( theGame.GetCurrentPlayer() );
			const Change& pool = theGame.GetGameChange(); 

			std::vector<Node>* moveListOut = new std::vector<Node>();
			
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
						moveListOut->push_back(m);
					}
				}
			}
			
			return moveListOut;
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
			
			std::vector<Node>* moveList = GetAllMoves(theGame);
			int best = 0;
			
			if (moveList->size()>1)
			{
				std::vector< float > uct;
				uct.resize( moveList->size(), 0 );

				int trials = 0;
				do
				{
					const float lnt = log(trials);
					int j=0;
					for (int i=0; i!=moveList->size(); ++i)
					{
						uct[i] = (*moveList)[i].UCT(lnt);
						if (uct[i]>uct[j]) j = i;
					}
				
					GameState newGame = theGame.PlayMove(  (*moveList)[j].mMove );
					 (*moveList)[j].mSims++;
					if (PlayOut(newGame)==theGame.GetCurrentPlayer())
					{
                        (*moveList)[j].mWins++;
						if (((*moveList)[j].Ratio()) > 
							((*moveList)[best].Ratio()))
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
			
			Move result = (*moveList)[best].mMove;
			delete moveList;
			
			mTimeTaken += times(&t) - currentTurnClockStart;
			return result;
		}

		void NotifyGameStart(const Game& theGame)
		{
			mTimeTaken = 0;
		}

		clock_t mTimeTaken;
		Change::List* mChangeList;

	// create an instance of this player
	} mcts2Player;
};
