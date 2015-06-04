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

namespace Thad{

	using namespace CPPFight;

	//
	// Monte Carlo tree search FIGHT Player
	//
	class MCTSBot : public Player
	{
	public:

		//construct base class with AI name and author name
		MCTSBot() : Player( "MCTS", "Thad" )
		{
		}

		void GetAllMoves( const GameState& theGame, std::vector<Move>& moveListOut )
		{
			const Change& player_change = theGame.GetPlayerChange( theGame.GetCurrentPlayer() );
			const Change& pool = theGame.GetGameChange(); 

			Change::List* someChange;
			if (recycledChangeLists.empty()){
				someChange = new Change::List;
			}
			else{
				someChange = recycledChangeLists.back();
				recycledChangeLists.pop_back();
			}

			// need to tell resize to will with a value even when count is 0
			moveListOut.resize(0, Move(PENNY));
			
			//for each coin that can be played
			for (const Coin *ci=&COINLIST[0];ci!=&COINLIST[COIN_COUNT];++ci){				
				if (player_change.GetCount(*ci) > 0){
					
					//get all possible sets of change into array
					GetAllPossibleChange(
						pool,
						*ci, 
						*someChange);

					//for each possible set of change
					const Change::List::reverse_iterator end = someChange->rend();
					for(Change::List::reverse_iterator i = someChange->rbegin();i!=end;++i){
				
						//create "move" from coin to give, and change to take				
						const Move m(*ci,*i);
						moveListOut.push_back(m);
					}
				}
			}

			recycledChangeLists.push_back( someChange );
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
			std::vector<Move> moveList;
			GetAllMoves(theGame, moveList);
			std::vector<int> scores;
			scores.resize( moveList.size(), 0 );

			int best = 0;
			const int trials  = 512;
			for (int a=0; a!=trials; ++a)
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
			}

			return moveList[best];
		}
		
		//virtual 
		//pass on success notfication to relevent Genepool
		void NotifyWon()
		{

		}

		//virtual 
		//pass on failure notfication to relevent Genepool
		void NotifyEliminated()
		{

		}

		void NotifyGameStart(const Game& theGame)
		{

		}

		//help memory management a little bit...
		std::vector< Change::List* > recycledChangeLists;

	// create an instance of this player
	} mctsPlayer;
};
