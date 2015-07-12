//
// "SmartyPants" - Thad's FIGHT! AI v2.5
// (c) T.Frogley 2001 (original implementation), 
// (c) T.Frogley 2015 (iterative deepening work)
// codemonkey.uk@gmail.com
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

#include "search_costs.h"
#define UPDATE_SEARCH_COSTS stderr

extern clock_t ticks_per_s;

namespace Thad{

	using namespace CPPFight;

    float GetRatio(int turn, int depth)
    {
        float result = 2.5f;
        if (turn < search_costs_t && depth <= search_costs_d)
        {
            float a = search_costs[turn][depth-1];
            float b = search_costs[turn][depth];
            if (a>0 && b>0)
                result = b / a;
        }
        return std::max(1.f, result);
    }
    
    // how many nodes will I inspect if I search to depth on turn
    int GetNodes(int turn, int depth)
    {
        // bring turn into range
        while (turn>=search_costs_t && turn>0)
            turn--;
    
        // depth array is 1-base
        int dm = 0;
        do {
            dm++;
            depth--;
            // bring depth into range, and find a data-point
        } while ((depth>=search_costs_d || search_costs[turn][depth]==0) && depth>0);

        // extrapolate for missing data
        int result = std::max(1, search_costs[turn][depth]);
        while (dm-- && result < std::numeric_limits<int>::max()/2)
            result += result*2;
        
        return result;
    }

	//
	// Genome type
	// Stores heuristics in a format that is robust with respect to mutations
	//
	class Genome {
	public:

		//creation of a random genome 
		Genome()
		{
			for (int i=0;i<COIN_COUNT;++i){
				data[i] = rand()/COIN_COUNT;
			}
			Normalise();
		}

		//cross two genomes
		Genome(const Genome& lhs, const Genome& rhs)
		{
			int i,e=rand()%COIN_COUNT;
			
			//"cross" combine
			for (i=0;i!=e;++i){
				data[i] = lhs.data[i];
			}
			for (;i!=COIN_COUNT;++i){
				data[i] = rhs.data[i];
			}
			
			//25% chance "mutate"
			if (rand()%4==0)
				data[rand()%COIN_COUNT]=data[rand()%COIN_COUNT];

			Normalise();
		}

		//create a genome from a heuristic "template"
		Genome(const int tmplate[COIN_COUNT])
		{
			data[0]=tmplate[0];
			data[1]=tmplate[1]-data[0];
			data[2]=tmplate[2]-data[1];
			data[3]=tmplate[3]-data[2];
			Normalise();
		}

		//generate the heuristic 
		void GetHeuristic(int target[COIN_COUNT], int scale=65536)
		{
			float total = 0;
			for (unsigned int i=0;i!=COIN_COUNT;++i){
				total += data[i] * scale;
				target[i] = total;
			}
		}

		//compare two Genomes (required for std::set<Genome>)
		bool operator<(const Genome& rhs)const
		{
			for (unsigned int i=0;i!=COIN_COUNT;++i){
				if (data[i] < rhs.data[i]) return true;
				if (data[i] > rhs.data[i]) return false;
			}
			return false;
		}

	private:
		float data[COIN_COUNT];

		//normalise the genome to the range 0..1
		//(helps keep Genome robust during cross mutation)
		void Normalise()
		{
			float total = 0;
			int i;
			for (i=0;i!=COIN_COUNT;++i){
				total = total + data[i];
			}
			for (i=0;i!=COIN_COUNT;++i){
				data[i] = data[i] / total;
			}
		}
	};

	class GenomePool
	{
	public:

		//create a gene pool, seeding it with the passed in genome set
		GenomePool(const std::set<Genome>& seed_pool, int size=20)
		{
			myGenomeData.reserve(size);

			for(std::set<Genome>::const_iterator i = seed_pool.begin();i!=seed_pool.end();++i){				
				myGenomeData.push_back( std::pair< Genome, float >(*i, 1.0) );
			}

			myGenomeData.resize(size);

			Repopulate();

			myIndex=0;
		}
		
		//find the current best genome, and return its index
		//used for seeding new gene pools, and reporting
		int GetCurrentBestGenome()
		{			
			int best=0;
			float score = myGenomeData[best].second;
			for (int j=1;j<=myIndex;++j){
				if (myGenomeData[j].second>score){
					best = j;
					score = myGenomeData[j].second;
				}
			}
			return best;
		}

		//returns a specific genome by index
		//used for seeding new gene pools, and reporting
		Genome GetGenome(unsigned int index)
		{
			if (index<myGenomeData.size()){
				return myGenomeData[index].first;
			}
			else throw Exception();
		}

		//Get the currently active genome's heuristic
		void GetHeuristic(int* target)
		{
			myGenomeData[myIndex].first.GetHeuristic( target );
		}

		//notify the gene pool of a specific genes success
		void Good()
		{
			//increment its win count, and keep using it
			myGenomeData[myIndex].second += 1;
		}

		//notify the gene pool of a specific genes failure
		void Bad(int survivalPeriod)
		{
			//failed ... but by how much?
			// - vital for developing a winning strategy
			myGenomeData[myIndex++].second -= 1.0/survivalPeriod;

			//go to next genome
			if (myIndex==myGenomeData.size()){
				//whole pool tested, return to start and repopulate
				myIndex=0;
				Repopulate();
			}
		}

	private:
		
		//kill off weekest portion of gene pool and repopulate with new / cross mutated
		void Repopulate()
		{
			const int size = myGenomeData.size();

			std::sort(myGenomeData.begin(),myGenomeData.end(),SortFn());
			myGenomeData.resize(size/4);
			
			while(myGenomeData.size()<3*size/4 )
					myGenomeData.push_back(
						std::pair<Genome, float>(
							Genome(
								myGenomeData[rand()%myGenomeData.size()].first,
								myGenomeData[rand()%myGenomeData.size()].first
							),
							0
						)
					);

			//pad to end with new (random) genes
			myGenomeData.resize( size );

			//zap scores
			std::for_each(myGenomeData.begin(),myGenomeData.end(),ResetFn());
		}
	
		//function objects for manipulating the genepool using std::algorithms
		struct SortFn{
			bool operator()(const std::pair<Genome, float>& a, const std::pair<Genome, float>& b){
				return a.second > b.second;
			}
		};
		struct ResetFn{
			void operator()(std::pair<Genome, float>& a){
				a.second = 0;
			}
		};

		//gene pool and index into it
		int myIndex;
		std::vector< std::pair<Genome, float> > myGenomeData;
	};

	//
	// SmartyPants AI Player
	//
	class SmartyPants : public Player
	{
	public:

		//construct base class with AI name and author name
		SmartyPants()
		 : Player( "SmartyPants", "Thad" )
		 , mDepth(1)
		 , mDefaultDepth(1)
		{
		}

		//destructor - report heuristics for possible reuse later
		~SmartyPants()
		{
            LogTimes();
#ifdef SMARTYPANTS_PRINT_HEURISTICS
			for(GenomePoolMap::iterator i=myGenomePool.begin();i!=myGenomePool.end();++i){
				int h[4];
				i->second.GetGenome(i->second.GetCurrentBestGenome()).GetHeuristic(h);
				printf("(%i,%i)=%i,%i,%i,%i\n",i->first.type,i->first.id,h[0],h[1],h[2],h[3]);
			}
#endif//SMARTYPANTS_PRINT_HEURISTICS
		}

		//calculate the heuristic value of a pile of change
		inline int ChangeHeuristic( const Change& change, const int* heuristic ) const 
		{
			int score = 0;
			for (const Coin *c=&COINLIST[0];c!=&COINLIST[COIN_COUNT];++c){
				score += *heuristic++ * change.GetCount(*c);
			}
			return score;
		}

		//calculate the heuristic value of a game state with respect to a specific player
		int GameHeuristic( const GameState& game, int forwhom, const int* heuristic ) const 
		{
			int result;
			result = ChangeHeuristic(game.GetPlayerChange( forwhom ), heuristic );

			int best = std::numeric_limits<int>::min();
			for(int i=0;i!=game.GetPlayerCount();++i){
				if (i!=forwhom){
					int score = ChangeHeuristic( game.GetPlayerChange( i ), heuristic );
					if (score>best){
						best = score;
					}
				}
			}

			return result - best;
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

			int bestScore = (player_index == theGame.GetCurrentPlayer())?
							std::numeric_limits<int>::min()
							:
							std::numeric_limits<int>::max();

			Coin bestCoin;
			Change bestChange;

			Change::List* someChange;
			if (recycledChangeLists.empty()){
				someChange = new Change::List;
			}
			else{
				someChange = recycledChangeLists.back();
				recycledChangeLists.pop_back();
			}

			//for each coin that can be played
			for (const Coin *ci=&COINLIST[0];ci!=&COINLIST[COIN_COUNT];++ci){
				if (player_change.GetCount(*ci) > 0){
					
					//get all possible sets of change into array
					GetAllPossibleChange(
						pool,
						*ci, 
						*someChange);
						
					FilterChangeInPlace(*someChange);

					//for each possible set of change
					//(reversed iteration improves AB prune by as much as 70% (unfiltered change set)
					const Change::List::reverse_iterator end = someChange->rend();
					for(Change::List::reverse_iterator i = someChange->rbegin();i!=end;++i){
				
						//create "move" from coin to give, and change to take
						const Move myMove(*ci,*i);
						
						//create gamestate once the move has been played
						GameState newGame = theGame.PlayMove( myMove );

						//get "score" for this move
						int score;
						if (recursive>0 && newGame.GetActivePlayers()>1){
							
							//recursivly examine move, get its score
							score = GetBestMove( newGame, player_index, bestScore, recursive-1).score;
						}
						else{

							//leaf node - just get heuristic value
							score = GameHeuristic( newGame, player_index, myHeuristic );						
							// count leaf nodes inspected
							mVA++;
						}
					
						//Min/Max search
						if ((player_index == theGame.GetCurrentPlayer() && score > bestScore) ||
							(player_index != theGame.GetCurrentPlayer() && score < bestScore))
						{
							bestScore  = score;
							bestCoin = *ci;
							bestChange = *i;

							//Alpha/Beta pruning
							if ((player_index == theGame.GetCurrentPlayer() && score > abPrune) ||
								(player_index != theGame.GetCurrentPlayer() && score < abPrune)){

								goto exit;

							}

						}
					}
				}
			}

exit:
			recycledChangeLists.push_back( someChange );
			return MoveScore( Move( bestCoin, bestChange ), bestScore );
		}

		//override GetMove function
		virtual Move GetMove (const Game& theGame)
		{
			//count turns for GA, then return best move according to min/max
			++myTurnCount;		
		
			// Timing stuff
			tms t;
			clock_t currentTurnClockStart = times(&t);
		
		    // plan on using 90% of game time, leaves 10% margin for error
			const clock_t timeLeft = (CFIGHT_PLAYER_TIME_PER_GAME - mTimeTaken) * 0.9f;

			// number of coins left == worst case scenario for turns I have left
			// using a worst case here front loads effort, 
			// so more time is spent considering moves in the early game
			const int turnsLeft = theGame.GetPlayerChange( 
				theGame.GetCurrentPlayer() 
			).GetTotalCount();
		
			// take an appropriate number of time given time left and turns left to play
			const clock_t turnTime = timeLeft / (turnsLeft+1);
		
			// value of coins left is worst case scenario for turns left in game
			int maxDepth = 0;
			for (int p=0;p!=theGame.GetPlayerCount();++p)
				maxDepth = theGame.GetPlayerChange( p ).GetTotalValue();
		
			// cap out mDepth so iterative deepening doesn't go wild at end-game
			if (maxDepth < mDepth)
				mDepth = maxDepth;

			clock_t dt = 0;
			clock_t searchTime = 0;
			Move result(PENNY);
			
			float node_time = 1.f;
            bool repeat = false;
			do
			{
				clock_t searchStartTime = currentTurnClockStart + dt;

                mVA = 0;
				result = GetBestMove( theGame, theGame.GetCurrentPlayer(), std::numeric_limits<int>::max(), mDepth ).move;
			
				clock_t timeNow =  times(&t);
				searchTime = std::max(1ul, timeNow - searchStartTime);
				
				LogVisits( theGame.GetCurrentTurn(), mDepth, mVA );
				
				// time since turn started
				dt = (timeNow - currentTurnClockStart);
				
			    node_time = (float)searchTime / (float)mVA;
				
				// decide if there is time to do a deeper search for the current turn
                repeat = false;
                while (dt + GetNodes(theGame.GetCurrentTurn(), mDepth+1)*node_time < turnTime && mDepth < maxDepth)
                {
                    mDepth ++;
                    repeat = true;
                }
                
			} while (repeat);
			
			// remember where we advanced depth to on our opening move...
			if (myTurnCount == 1)
				mDefaultDepth = mDepth;
			
			mTimeTaken += dt;
			if (mTimeTaken>CFIGHT_PLAYER_TIME_PER_GAME)
			{
			    fprintf(stderr, 
			        "%s is about to be eliminated, on turn %i (%i) spent %f searching to depth = %i / %i\n"
			        "%i nodes visited, expected %i\n"
    			    "turn time budget was %fs / %fs with ~%i turns left.\n",
			        GetTitle().c_str(),
			        myTurnCount, theGame.GetCurrentTurn(), 
			        searchTime/(float)ticks_per_s,
			        mDepth, maxDepth, 
			        mVA, GetNodes(theGame.GetCurrentTurn(), mDepth),
			        turnTime/(float)ticks_per_s, timeLeft/(float)ticks_per_s, 
			        turnsLeft);
			}
			 
			// back off for next turn
            while (GetNodes(theGame.GetCurrentTurn()+theGame.GetPlayerCount(), mDepth)*node_time > turnTime && mDepth>1)
            {
                mDepth--;
			}
				
			return result;
		}
		
		//virtual 
		//pass on success notfication to relevent Genepool
		void NotifyWon()
		{
			GenomePoolMap::iterator itr = myGenomePool.find(myGameKey);
			if(itr==myGenomePool.end()) 
				throw Exception();
			itr->second.Good();
		}

		//virtual 
		//pass on failure notfication to relevent Genepool
		void NotifyEliminated()
		{
			GenomePoolMap::iterator itr = myGenomePool.find(myGameKey);
			if(itr==myGenomePool.end()) 
				throw Exception();
			itr->second.Bad(myTurnCount);
		}

		void NotifyGameStart(const Game& theGame)
		{
			//reset turn counter
			myTurnCount = 0;
			
			mDepth = mDefaultDepth;
			mTimeTaken = 0;

			//generate game key
			if (theGame.GetPlayerCount()==2){
				myGameKey.type = GameKey::ONE_ON_ONE;
				for(int i=0;i<theGame.GetPlayerCount();++i)
					if(theGame.GetPlayerUID(i)!=GetUID())
						myGameKey.id = theGame.GetPlayerUID(i);
			}
			else{
				myGameKey.type = GameKey::MULTIPLAYER;
				
				//find position in circle
				int i;
				for(i=0;i<theGame.GetPlayerCount();++i)
					if(theGame.GetPlayerUID(i)==GetUID())
						break;

				//encode number of players, and position in circle into game key
				myGameKey.id = theGame.GetPlayerCount()*theGame.GetPlayerCount()+i;
			}
			
			//check of gene pool for this key exists
			GenomePoolMap::iterator itr = myGenomePool.find(myGameKey);
			if(itr==myGenomePool.end()){

				//
				//create new gene pool for new key
				//

				std::set< Genome > seed_pool;
				
				//pull in recent experiance
				for(GenomePoolMap::iterator i=myGenomePool.begin();i!=myGenomePool.end();++i){
					seed_pool.insert( i->second.GetGenome(i->second.GetCurrentBestGenome()) );
				}

				//insert some historically good heuristics
				
				//strong 1 on 1
				const static int recyled1[4] = { 6979, 13665, 30386, 65535 };
				seed_pool.insert(recyled1);

				//strong 4 player heuristic
				const static int recyled4[4] = { 8818, 17266, 29574, 65535 };
				seed_pool.insert(recyled4);

				//create a new gene pool using these seeds and add it to the map
				itr = myGenomePool.insert( 
					myGenomePool.begin(), 
					GenomePoolMap::value_type( myGameKey, GenomePool( seed_pool ) ) 
				);
			}
			
			//get heuristic for this game
			itr->second.GetHeuristic( myHeuristic );
		}

		//game key - type and id
		struct GameKey{
			enum Type { ONE_ON_ONE, MULTIPLAYER } type;
			int id;
			bool operator<(const GameKey& rhs)const
			{
				return type<rhs.type || (type==rhs.type && id<rhs.id);
			}
		} myGameKey;

		//current heuristic
		int myHeuristic[4];

		//current turn count (for learning AI)
		int myTurnCount;
		
		int mDepth, mDefaultDepth;

		//note - do not use [] as GenomePool as no default constructor
		typedef std::map< GameKey, GenomePool > GenomePoolMap;
		GenomePoolMap myGenomePool;

		//help memory management a little bit...
		std::vector< Change::List* > recycledChangeLists;

		// track time taken this game, for iterative deepening
		clock_t mTimeTaken;
		int mVA; // visit accumulator
		
#ifdef UPDATE_SEARCH_COSTS 
        std::map< std::pair<int, int>, int > mVisits;
        static int maxTurn;
        static int maxDepth;
		void LogVisits(int turn, int depth, int nodes)
		{
            maxTurn = std::max(maxTurn, turn);
            maxDepth = std::max(maxDepth, depth);
            // get worst case times at depths
            mVisits[ std::pair< int, int>(turn, depth) ] = std::max(
                mVisits[ std::pair< int, int>(turn, depth) ],
                nodes
            );
		}
		void LogTimes()
		{
		    fprintf(UPDATE_SEARCH_COSTS, "const int search_costs_t = %i;\n", maxTurn+1);
		    fprintf(UPDATE_SEARCH_COSTS, "const int search_costs_d = %i;\n", maxDepth);
		    fprintf(UPDATE_SEARCH_COSTS, "const int search_costs[search_costs_t][search_costs_d] = {\n");
		    for (int t=0;t<=maxTurn;++t)
		    {
		        fprintf(UPDATE_SEARCH_COSTS, "// t = %i \n{ ", t);
                for (int d=1;d<=maxDepth;++d)
                {
                    int dn = mVisits[ std::pair< int, int>(t, d) ];
                    if (d<=search_costs_d && t<search_costs_t)
                        dn = std::max(dn, search_costs[t][d-1]);
                    fprintf(UPDATE_SEARCH_COSTS, "%i, ", dn );
                }
                fprintf(UPDATE_SEARCH_COSTS, "}, \n");
		    }
		    fprintf(UPDATE_SEARCH_COSTS, "}; \n");
		}
#else
    inline void LogVisits(int turn, int depth){}
    inline void LogTimes(){}
#endif

	// create an instance of this player
	} maxChangePlayer;
#ifdef UPDATE_SEARCH_COSTS 
    int SmartyPants::maxTurn=search_costs_t-1;
    int SmartyPants::maxDepth=search_costs_d;
#endif
};
