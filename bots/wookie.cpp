// First attempt at getting an AI to work
// Created by Wookie 29/10/2001
//
#include "cppFIGHT.h"

namespace Wookie{

	using namespace CPPFight;
	
	class WookiePlayer2 : public Player
	{
		public:
			
			WookiePlayer2() : Player( "That's not AI!", "Wookie" )
			{
			}

			virtual Move GetMove( const Game& theGame )
			{
				const Change& pot  = theGame.GetGameChange(); // What's in the pot
				const Change& hand = theGame.GetPlayerChange(theGame.GetCurrentPlayer());
			
				// If the number of coins that will be won is greater than 2*CoinValue / 5
				// or the value of the coins adds up to greater than 2*CoinValue / 3
				for (int i = (CPPFight::COIN_COUNT - 1); i > 0; --i)
				{
					Coin testCoin = COINLIST[i];
					int test2Coin = (2 * ((int)testCoin));
					int coinNumMin = test2Coin / 5;
					int coinValMin = test2Coin / 3;

					const Change& tempChange = MaximumChangeValue(pot, testCoin);
					if ((hand.GetCount(testCoin) > 0) && 
						((tempChange.GetTotalCount() > coinNumMin) ||
						(tempChange.GetTotalValue() > coinValMin))	)
					{
						return Move(testCoin,tempChange);
					} 	
				}
			
				// if there are 5's in hand, and pennies in the pot, then better to 
				// take the pennies than throw out.
				if ((hand.GetCount(CPPFight::NICKEL)>0) && (pot.GetCount(CPPFight::PENNY)>0)) {	
					return Move(CPPFight::NICKEL, MaximumChangeValue(pot, CPPFight::NICKEL));
				}

				// Try to throw out a penny
				if (hand.GetCount(CPPFight::PENNY)>0) {	
					return CPPFight::PENNY;
				}

				// Get the most coins
				Coin bestCoin = PENNY;
				int grabCount = -1;
				for (int i = 0; i < CPPFight::COIN_COUNT; ++i){
					Coin testCoin = COINLIST[i];
					if (hand.GetCount(testCoin) > 0)
					{
						int tempChangeCount = MaximumChangeValue(pot, testCoin).GetTotalCount();
						if (tempChangeCount > grabCount)
						{
							bestCoin = testCoin;
							grabCount = tempChangeCount;
						}
					}
				}
									
				return Move(bestCoin,MaximumChangeValue(pot, bestCoin));
			}			
	} WookiePlayer2;
}
