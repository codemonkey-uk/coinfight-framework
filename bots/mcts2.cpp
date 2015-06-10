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
#include <cfloat>
#include <cassert>

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
		        , mSims(0)
		        , mChildren(0)
		    { }
		    
		    Move mMove;
		    int mWins;
		    int mSims;
		    
		    float UCT(float lnt) const
		    {
		        if (mSims==0) return FLT_MAX;
		        float uct = (float)mWins / (float)mSims;
		        uct += uct_c * sqrt(lnt / (float)mSims);
		        return uct;
		    }
		    
		    float Ratio() const
		    {
		        return (float)mWins / (float)mSims;
		    }
		    
		    std::vector<Node>* mChildren;
		};
		
		int CountTrials(const std::vector<Node>* nodes)
		{
		    int result=0;
		    for (int i=0;i!=nodes->size();++i)
		        result += (*nodes)[i].mSims;
		    return result;
		}
		
		void Cleanup(const std::vector<Node>* nodes)
		{
		    for (int i=0;i!=nodes->size();++i)
		    {
		        if ((*nodes)[i].mChildren)
		            Cleanup((*nodes)[i].mChildren);
		    }
		    delete nodes;
		}
		
		Node* SelectNode(std::vector<Node>* nodes)
		{
		    assert( nodes );
		    assert( nodes->empty()==false );
		    
            float best_uct = -FLT_MAX;
            Node* result = 0;
            const float lnt = log( (float)CountTrials(nodes) );
            for (std::vector<Node>::iterator i=nodes->begin(); i!=nodes->end(); ++i)
            {
                float uct = i->UCT(lnt);
                if (uct>best_uct) 
                {
                    result = &*i;
                    best_uct = uct;
                }
            }
            
            assert(result);
            return result;
		}

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
			std::random_shuffle( moveListOut->begin(), moveListOut->end() );
			return moveListOut;
		}

		int Explore( Node* node, GameState theGame )
		{
		    assert(node);
		    
    		int p = theGame.GetCurrentPlayer();
    		if (theGame.GetActivePlayers()==1)
        		return p;

            if (node->mChildren == 0)
                node->mChildren = GetAllMoves(theGame);
            
            node = SelectNode(node->mChildren);
            theGame = theGame.PlayMove( node->mMove );
            node->mSims++;
            int winner = Explore(node, theGame);
            node->mWins += (winner==p);

            return winner;
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
			assert( moveList && moveList->empty()==false );
			Node* best = &(*moveList)[0];
			
			if (moveList->size()>1)
			{
				do
				{
					Node* trial = SelectNode(moveList);
				
					GameState newGame = theGame.PlayMove( trial->mMove );
					trial->mSims++;
					if (Explore(trial, newGame)==theGame.GetCurrentPlayer())
					{
                        trial->mWins++;
						if (trial->Ratio() > 
							best->Ratio())
							best = trial;
					}
				
				}while( times(&t) - currentTurnClockStart < turnTime );
						
				#ifdef MCTS_LOG_RATIOS
				float trials = (float)CountTrials(moveList);
				Serialise(MCTS_LOG_RATIOS, theGame);
				for (int i=0; i!=moveList->size(); ++i)
				{
					int a = (*moveList)[i].mWins / trials * 80;
					int b = (*moveList)[i].mSims / trials * 80;
					char txt[80];
					std::fill(txt, txt+a, '+');
					std::fill(txt+a, txt+b, '-');
					txt[b]=0;
					fprintf(
					    MCTS_LOG_RATIOS, 
					    "%2i]%s%c\n", 
					    i, txt, (best==&(*moveList)[i])?'*':' '
					);
				}
				Serialise(MCTS_LOG_RATIOS, best->mMove);
				#endif
			}
			
			Move result = best->mMove;
			Cleanup( moveList );
			
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
