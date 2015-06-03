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

		//utility struct, for returning both a move, and its heuristic "score"
		struct MoveScore{
			MoveScore(Move m, int s) : move(m),score(s) {};

			const Move move;
			const int score;
		};

		//Get Best Move - 
		//recursive min/max algorithm explores game states and 
		//returns a move and its score, for a specific player
		MoveScore GetBestMove( const GameState& theGame, const int player_index, int abPrune, const int recursive=1)
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

			std::vector<Move> moveList;
			
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
						const Move myMove(*ci,*i);
						
						moveList.push_back(myMove);
						
						//create gamestate once the move has been played
						// GameState newGame = theGame.PlayMove( myMove );

					}
				}
			}

			recycledChangeLists.push_back( someChange );
			
			// random move
			return MoveScore( moveList[rand()%moveList.size()], 1 );
		}

		//override GetMove function
		virtual Move GetMove (const Game& theGame)
		{	
			return GetBestMove( theGame, theGame.GetCurrentPlayer(), std::numeric_limits<int>::max(), 3 ).move;
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
