//
// example player code for cppFIGHT v1.6c and above
// by Thaddaeus Frogley, Oscar Cooper and Martin Lester
//   Build, & link with 
//     cppFIGHT.cpp, 
//	   cexample.c, &
//	   cplayers.c
// See also cppFIGHT.h
//

#include "cppFIGHT.h"

//namespace for ExamplesTeam
namespace ExamplesTeam{

	// includes examples by 
	//   Thaddaeus Frogley, 
	//   Oscar Cooper, &
	//   Martin Lester.

	//import the CPPFight namespace
	using namespace CPPFight;
	
	//
	// RandomPlayer - an example AI by Thaddaeus Frogley
	// "stupid AI plays semi-random legal moves"
	class RandomPlayer : public Player
	{
		public:
			
			//construct base class with AI name and author name
			RandomPlayer() : Player( "Randy", "Examples Team" )
			{
			}

			//override GetMove function
			virtual Move GetMove( const Game& theGame )
			{
				//Get the change for this player, and the change in the center of the table
				const Change& myChange   = theGame.GetPlayerChange(theGame.GetCurrentPlayer());
				const Change& gameChange = theGame.GetGameChange();
				
				//select a random offset into the coin list
				int r = rand();

				//iterate over the coin list
				for (int coinIndex = 0; coinIndex < CPPFight::COIN_COUNT; ++coinIndex){
					
					//get a coin from a wrapped offset within the coin list,
					Coin testCoin = COINLIST[(coinIndex + r)%COIN_COUNT];

					//if the player has the coint available to them
					if (myChange.GetCount(testCoin) > 0){
						
						//on a 50/50 coin flip
						if (rand()%2){
							
							//play that coin, and try to minimise the value of the loss
							return Move(testCoin,MaximumChangeValue(gameChange, testCoin));
						}
						else{

							//play that coin, and get as many coins as possible
							return Move(testCoin,MaximumChangeCount(gameChange, testCoin));
						}
					}
				}

				//well, if the code got here its all gone wrong
				throw Exception();
			}

	// create an instance of this player
	} randomPlayer;

	//
	// MinLossGreedyPlayer - an example AI origanally by Oscar Cooper
	// "simple AI attempts to minimise the amount of loss per move"
	class MinLossGreedyPlayer : public Player
	{
	public:
		
		//construct base class with AI name and author name
		MinLossGreedyPlayer() : Player( "Greedy", "Examples Team" )
		{
		}

		//override GetMove function
		virtual Move GetMove (const Game& theGame)
		{
			//Get my player index
			const int myID = theGame.GetCurrentPlayer();
			
			//Get my change, and the change in the center of the table
			const Change& myChange   = theGame.GetPlayerChange(myID);
			const Change& gameChange = theGame.GetGameChange();

			int    bestLoss  = -1;
			int    bestCount = 0;
			Coin   bestCoin;
			Change bestChange;

			//iterate over the coin list
			for (int coinIndex = 0; coinIndex < CPPFight::COIN_COUNT; ++coinIndex)
			{
				//if I have one of these coins
				const Coin testCoin = COINLIST[coinIndex];
				if (myChange.GetCount(testCoin) > 0)
				{
					//find out how much change (value) can be got for this coin
					const Change testChange = MaximumChangeValue(gameChange, testCoin);

					//calulate the loss for playing this coin
					const int loss  = testCoin - testChange.GetTotalValue();

					//find out how many coins this change has in it
					const int count = testChange.GetTotalCount();

					//if this is the minimum loss, 
					//or is equal to the minimum loss, but has more coins
					if (((loss < bestLoss) || (bestLoss < 0)) ||
						((loss == bestLoss) && (count > bestCount)))
					{
						//remember this move
						bestLoss   = loss;
						bestCount  = count;
						bestCoin   = testCoin;
						bestChange = testChange;
					}
				}
			}

			//play the "best" move encountered
			return Move(bestCoin, bestChange);
		}

	// create an instance of this player
	} minLossGreedyPlayer;

	//
	// MaxChangePlayer - an example AI by Thaddaeus Frogley
	// "simple AI attempts to maximise the number of coins in hand"
	class MaxChangePlayer : public Player
	{
	public:

		//construct base class with AI name and author name
		MaxChangePlayer() : Player( "Coinage", "Examples Team" )
		{
		}

		//override GetMove function
		virtual Move GetMove (const Game& theGame)
		{
			//Get my player index
			const int myID = theGame.GetCurrentPlayer();
			
			//Get my change, and the change in the center of the table
			const Change& myChange   = theGame.GetPlayerChange(myID);
			const Change& gameChange = theGame.GetGameChange();

			//test change (outside loop as optimisation)
			std::vector< Change > someChange;

			int    bestLoss;
			int    bestCount = -1;
			Coin   bestCoin;
			Change bestChange;

			//iterate over the coin list
			for (int coinIndex = 0; coinIndex < CPPFight::COIN_COUNT; ++coinIndex)
			{
				//if I have one of these coins
				const Coin testCoin = COINLIST[coinIndex];				
				if (myChange.GetCount(testCoin) > 0)
				{
					//find out what coins I can get for this coin, 					
					GetAllPossibleChange(
						gameChange,
						testCoin, 
						someChange);

					for(std::vector< Change >::iterator i = someChange.begin();i!=someChange.end();++i){
						
						//how many coins, and what the loss (value) would be
						const Change testChange = *i;
						const int loss  = testCoin - testChange.GetTotalValue();
						const int count = testChange.GetTotalCount();

						//if the most coins so far, or the same coins with more value
						if ((count > bestCount) ||
							(count == bestCount && (loss < bestLoss)))
						{
							//remember this move
							bestLoss   = loss;
							bestCount  = count;
							bestCoin   = testCoin;
							bestChange = testChange;
						}
					}
				}
			}

			//play the "best" move encountered
			return Move(bestCoin, bestChange);
		}

	// create an instance of this player
	} maxChangePlayer;

	//
	// AllHigh - an example AI origanally by Martin Lester
	// "always plays the highest coin in hand, takes change by value"
	class AllHigh : public Player
	{
		public:

			//construct base class with AI name and author name
			AllHigh() : Player( "AllHigh", "Examples Team" )
			{
			}

			//override GetMove function
			virtual Move GetMove( const Game& theGame )
			{
				//get players change
				const Change& hand = theGame.GetPlayerChange(theGame.GetCurrentPlayer());

				//find highest value coin in hand by searching in reverse order
				Coin highest = QUARTER;								
				for (int i = COIN_COUNT-1; i >= 0; --i)
				{
					if (hand.GetCount(COINLIST[i]) > 0) {
						highest = COINLIST[i];
						break;
					}
				}
				
				//play that coin, and some change from pot
				return Move(highest, MaximumChangeValue(theGame.GetGameChange(), highest));
			}

	// create an instance of this player
	} AllHigh;

	//
	// AllHigh - an example AI origanally by Martin Lester
	// "always plays the lowest value coin in hand, takes change by count"
	class AllLow : public Player
	{
		public:
			
			//construct base class with AI name and author name
			AllLow() : Player( "AllLow", "Examples Team" )
			{
			}
			
			//override GetMove function
			virtual Move GetMove( const Game& theGame )
			{
				const Change& hand = theGame.GetPlayerChange(theGame.GetCurrentPlayer());

				//find lowest value coin in hand
				Coin lowest = CPPFight::PENNY;	
				for (int i = 0; i < CPPFight::COIN_COUNT; ++i)
				{
					if (hand.GetCount(CPPFight::COINLIST[i]) > 0) {
						lowest = CPPFight::COINLIST[i];
						break;
					}
				}
				
				return Move(lowest, MaximumChangeCount(theGame.GetGameChange(), lowest));
			}

	// create an instance of this player
	} AllLow;

}//namespace ExamplesTeam