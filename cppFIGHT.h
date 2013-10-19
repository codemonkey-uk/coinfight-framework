// cppFIGHT v1.8
// An AI Tournement Framework for "FIGHT!", a free game from Cheapass Games
// (c) T.Frogley 2002-2013
//   http://thad.frogley.info/
//   mailto:codemonkey.uk@gamail.com
//
// FIGHT! (c) Cheapass Games
//   http://www.cheapass.com/fight.shtml
//
// With thanks also to Oscar Cooper
//
// Players: 2-6 
// Object:  To run everyone else out of coins. 
// To Begin: 
//   Each player starts with one quarter (1x25p), two dimes (2x10p), 
//   three nickels (3x5p), and four pennies (4x1p)
//
// On Each Turn: 
//   Play one coin from your pile into the middle of the table. 
//   You may then take change from the table, up to a 1 less than the value 
//   of the coin you played.  For example, if you play a dime (10p), you can 
//   take out up to 9p in change, if there is that much to be taken. 
//
// To Win: 
//   Be the last player with any coins left. 

//
// see also:
//   example.cpp for example AI(s) in C++
//   cFIGHT.h for C API
//   cexample.c for example AI(s) in C

#ifndef CPPFIGHT_HEADER_INCLUDED
#define CPPFIGHT_HEADER_INCLUDED

#ifdef _MSC_VER 
	#pragma warning( disable : 4786 )
#endif

#include <vector>
#include <string>
#include <ctime>

#include "cFIGHT.h"

namespace CPPFight
{
	//
	// Coin - game token with value
	//
	typedef CFIGHT_Coin Coin;
	
	//types of token with specified values
	const Coin PENNY   =  CFIGHT_PENNY;
	const Coin NICKEL  =  CFIGHT_NICKEL;
	const Coin DIME    =  CFIGHT_DIME;
	const Coin QUARTER =  CFIGHT_QUARTER;

	//types of token (for iterating over - you know you want to!)
	const int COIN_COUNT = CFIGHT_COIN_COUNT;
	const Coin COINLIST[ COIN_COUNT ] = { PENNY, NICKEL, DIME, QUARTER };

	//how many coins players start with, coresponds to array above
	const int INITAL_PLAYER_COINS[ COIN_COUNT ] = { 
		4,	//PENNY, 
		3,	//NICKEL, 
		2,	//DIME, 
		1,	//QUARTER 
	};

	//get the location of a coin within the above list
	int GetCoinIndex( Coin coinType );

	//exception classes - (more to come later)
	class Exception
	{};

	class OutOfTimeException : public Exception
	{};

	//
	// Change - some change, a collection of coins
	//

	class Change {
		public:

			typedef std::vector<Change> List;

			Change();
			Change(const Change& c);
					
			// interogate change

			int GetCount(Coin c) const;	//number of coins of type
			int GetValue(Coin c) const;	//value of coins of type

			int GetTotalValue() const;	//total value of change
			int GetTotalCount() const;	//total number of coins

			bool IsEmpty() const;		//any coins at all?

			// manipulate change

			Change& InsertCoins(Coin c, unsigned int n);
			Change& RemoveCoins(Coin c, unsigned int n);

			Change& InsertCoin(Coin c);
			Change& RemoveCoin(Coin c);

			Change& InsertChange(const Change& c);
			Change& RemoveChange(const Change& c);

			//+= operator, syntactic sugar for InsertChange
			inline Change& operator+=(const Change& rhs)
			{ return InsertChange(rhs); }
			
			//+= operator, syntactic sugar for RemoveChange
			inline Change& operator-=(const Change& rhs)
			{ return RemoveChange(rhs); }

			//support for C API / data structures
			explicit Change(const CFIGHT_Change& c);
			void GetCChange(CFIGHT_Change* result)const;

		private:
			unsigned int myChange[COIN_COUNT];
	};

	//
	// Change making functions
	// Note:  May be depricated in favor of Change member functions
	//		  TBD - you input is welcomed!
	//

	// gets the largest value of change possible from a coin 
	Change MaximumChangeValue (const Change& availableChange, 
		                       const Coin coinToChange);

	// gets the largest number of coins in change possible from a coin
	Change MaximumChangeCount (const Change& availableChange, 
		                       const Coin coinToChange);

	// finds all possible change combinations from the given 
	// pool, up to the maximum value specified
	// IN availableChange - change available to be taken
	// IN maximumValue - maximum value of change that may be taken
	// OUT - all possible change combinations
	void GetAllPossibleChange(
		const Change& availableChange, 
		const int maximumValue, 
		Change::List& result
	);

	//as above, but passed coin to change up to, 
	//rather than upper of returned change limit
	inline void GetAllPossibleChange(
		const Change& availableChange, 
		const Coin coinToChange, 
		Change::List& result
	) { GetAllPossibleChange( availableChange, coinToChange-1, result ); }

	// adding change, combine two lots of change (insertion)
	// ie Change c = a+b;
	inline Change operator+(Change lhs, const Change& rhs)
	{ return lhs.InsertChange( rhs ); }

	// subtracting change
	// ie Change c = a-b;
	inline Change operator-(Change lhs, const Change& rhs)
	{ return lhs.RemoveChange( rhs ); }

	// adding a coin to change, 
	// ie Change c = a+b;
	inline Change operator+(Change lhs, const Coin& rhs)
	{ return lhs.InsertCoin( rhs ); }

	// subtracting a coin from change, 
	// ie Change c = a-b;
	inline Change operator-(Change lhs, const Coin& rhs)
	{ return lhs.RemoveCoin( rhs ); }

	void Serialise(FILE* f, const Change& change);
	bool Serialise(FILE* f, Change* change);
	
	//
	// Move - each move consists of a coin given, and change taken.  The 
	//        value of the change must be less than the value of the coin
	//
	class Move{
		public:
			
			//illegal moves should be imposible to construct
			class Illegal : public Exception {};
			
			//construct move
			Move(Coin give, const Change& take);
			Move(Coin give);
			
			//copy move
			Move(const Move& other):
				myGive(other.myGive),
				myTake(other.myTake)
				{ }			
			Move& operator=(const Move& other)
			{ myGive=other.myGive; myTake = other.myTake; return *this; }

			//examine move
			inline const Coin& GetCoin() const
			{	return myGive;	}
			inline const Change& GetChange() const
			{	return myTake;	}

			//C API / data structure support
			explicit Move(const CFIGHT_Move& cmove);

		private:
			Coin myGive;
			Change myTake;
	};

	void Serialise(FILE* f, const Move& move);
	
	//
	// Game - (decleration required by Player class, defined later)
	//
	class Game;

	//
	// Base class for your AI players
	// derive from this class, and create an instance of it, 
	// it will be automatically entered in the tournement
	// See also, example code
	//

	class Player : public CFIGHT_Player
	{
		public:

			//pass your entry's title, and your name
			Player( const std::string& title, const std::string& author );
			
			// the main hoo-har, examine the game, and return your move...
			virtual Move GetMove( const Game& theGame )=0;

			// optional overloads
			// notifies player of game status, start, win/loose
			// useful for learning AI's
			virtual void NotifyGameStart(const Game& theGame);
			virtual void NotifyWon();
			virtual void NotifyEliminated();
			
			//depricated
			virtual void NotifyGameStart();
			
			
			// accessors for Title Author and UID
			inline const std::string& GetTitle()const
			{ return myTitle; }
			inline const std::string& GetAuthor()const
			{ return myAuthor; }
			inline int GetUID()const
			{ return myUID; }
			
			//dtor, you know the score.
			virtual ~Player();

		private:
			const int myUID;
			const std::string myTitle, myAuthor;

			static int theNextUID;
	};

	//
	// GameState - stores game state (change on table, change in player hands)
	// Essentially a const object, modification is not permited,
	// BUT you can get a NEW copy of the game state, as it would be after a move has been played.
	//

	class GameState : public CFIGHT_GameState{
		public:
			
			//
			// Game functionality for players
			//

			int GetPlayerCount() const;		// how many players in the game
			int GetCurrentPlayer() const;	// the current player index (ie you, if calling from inside Player::GetMove)
			int GetCurrentTurn() const;
			
			//for iterating over players
			int GetNextPlayer(int currentPlayerIndex) const;
			int GetNextActivePlayer(int currentPlayerIndex) const;

			int GetActivePlayers()const; // the number of players *still active* in game			
			
			bool IsPlayerActive(int player ) const;	//true if a player (by index) has any change left
			const Change& GetPlayerChange( int player ) const; // change pool for a specific player (by index)
			
			const Change& GetGameChange () const; // change pool currently available

			GameState PlayMove( Move move ) const;

		protected:
			GameState(int player_count);
			GameState(FILE*);

			int myTurn;
			Change myChange;
			std::vector< Change > myPlayersChange;		
	};

	// Extends GameState to add a UID for a specific player (by index) 
	// - which is consistant from game to game
	//   and time used so far
	class Game : public GameState, public CFIGHT_Game {
		public:
			Game(int player_count);
			Game(FILE *f, int player_count, int turn);
			virtual int GetPlayerUID( int player ) const = 0; 
			virtual clock_t GetPlayerTimeTaken( int player ) const = 0;
			virtual ~Game()	{}
	};

	void Serialise(FILE* f, const GameState& gameState);
	
	const clock_t PLAYER_TIME_PER_GAME = CFIGHT_PLAYER_TIME_PER_GAME;

}//namespace CPPFight

#endif//CPPFIGHT_HEADER_INCLUDED