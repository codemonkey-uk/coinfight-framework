/*
  C API header for C++ Fight
  cFIGHT.h v1, for cppFIGHT v1.8

  (c) T.Frogley 2002-2013
    http://thad.frogley.info
    mailto:codemonkey.uk@gmail.com

  FIGHT! (c) Cheapass Games
    http://www.cheapass.com/fight.shtml

  see also cexample.cpp for example AI,
  add function call to cplayers.c to include C AIs
*/

#ifndef CFIGHT_H_INCLUDED
#define CFIGHT_H_INCLUDED

#include <time.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum { 
	CFIGHT_PENNY   =  1,
	CFIGHT_NICKEL  =  5,
	CFIGHT_DIME    = 10,
	CFIGHT_QUARTER = 25
} CFIGHT_Coin;

#define CFIGHT_COIN_COUNT 4 
#define CFIGHT_PLAYER_TIME_PER_GAME CFIGHT_GetPlayerTicksPerGame()

int CFIGHT_GetPlayerTicksPerGame();

/* mapping coins to indexes, and vice versa */
int CFIGHT_Coin2Index( CFIGHT_Coin coinType );
CFIGHT_Coin CFIGHT_Index2Coin( int i );

/* how many of each coin (by index) a player starts the game with */
int CPPFight_GetInitialPlayerCoins( int i );


/*
	struct CFightChange
*/
typedef struct { 
	/* 
		use CFIGHT_Coin2Index and CFIGHT_Index2Coin to map coins to location in this array
	*/
	unsigned int change[CFIGHT_COIN_COUNT]; 
	
} CFIGHT_Change;

/* get the total value of coins in Change */
int CFIGHT_Change_GetTotalValue( const CFIGHT_Change* change );

/* get the total value of coins in Change */
int CFIGHT_Change_GetTotalCount( const CFIGHT_Change* change );

/* add coins to change */
void CFIGHT_Change_InsertCoins( CFIGHT_Change* change, CFIGHT_Coin c, unsigned int n );

/* remove coins from change (returns number of coins removed, does not remove partial amounts) */
int CFIGHT_Change_RemoveCoins( CFIGHT_Change* change, CFIGHT_Coin c, unsigned int n );

/* get the largest value of change possible from a coin */
void CFIGHT_MaximumChangeValue (const CFIGHT_Change* availableChange, CFIGHT_Coin coinToChange, CFIGHT_Change* result);

/* gets the largest number of coins in change possible from a coin */
void CFIGHT_MaximumChangeCount (const CFIGHT_Change* availableChange, CFIGHT_Coin coinToChange, CFIGHT_Change* result);

/*
	Move Object Functions
	Note, all functions that return CPPFight_Move pass ownership of the object to the calling code
*/

typedef struct { 
	
	CFIGHT_Coin play_coin;
	CFIGHT_Change take_change;

} CFIGHT_Move;

/*
	C Player type
	Ownership of the CFIGHT_Player is managed by the application framework
*/

typedef struct {
	int dummy;	/*do not use*/
} CFIGHT_Player;

/*
	C Game type
	Ownership of the CFIGHT_Game is managed by the application framework
*/

typedef struct {
	int dummy;	/*do not use*/
} CFIGHT_Game;

typedef CFIGHT_Game* CPPFight_Game;
typedef CFIGHT_Player* CPPFight_Player;

/* 
	Callback function types 
	When implementing these callbacks, by convention use the form:
	[AUTHOR]_CFIGHT_[PLAYER_NAME]_[FUNCTION_TYPE], ie
	ExampleTeam_CFIGHT_CPlayer_MoveFn
*/

typedef CFIGHT_Move (*CFIGHT_Player_MoveFn)(CPPFight_Player, CPPFight_Game);
typedef void (*CFIGHT_Player_NotifyGameStartFn)(CPPFight_Player, CPPFight_Game);
typedef void (*CFIGHT_Player_NotifyWonFn)(CPPFight_Player);
typedef void (*CFIGHT_Player_NotifyEliminatedFn)(CPPFight_Player);
typedef void (*CFIGHT_Player_FreeCustomDataFn)(void*);

/* 
	configuration struct, only pGetMove is required, the remainder should be set to 0 when not in use 
*/

typedef struct {
	CFIGHT_Player_MoveFn pGetMove;
	CFIGHT_Player_NotifyGameStartFn pNotifyGameStart;
	CFIGHT_Player_NotifyWonFn pNotifyWon;
	CFIGHT_Player_NotifyEliminatedFn pNotifyEliminated;
	CFIGHT_Player_FreeCustomDataFn pFreeCustomData;
	void* pCustomData;
} CPPFight_CPlayerCfg;

/* 
	create a player 
	You need to do this to register your AI code with the tournement
	This should be done in your Create function, 
	which will be called by CFIGHT_CreateAllPlayers in cplayers.c
	See cexample.c and cplayers.c for more information
*/
void CPPFight_New_Player(const char * title, const char * author, CPPFight_CPlayerCfg cfg);

/* from a player (passed to callback functions), get the UID */
int CPPFight_Player_GetUID(CPPFight_Player the);

/* from a player (passed to callback functions), get the custom data ptr passed in as cfg */
void* CPPFight_Player_GetCustomData(CPPFight_Player the);

/*
  game state
  interogate for game info details 
*/

typedef struct {
	int dummy; /*do not use*/
} CFIGHT_GameState;

typedef CFIGHT_GameState* CPPFight_GameState;

/* how many players in the game */
int CPPFight_GameState_GetPlayerCount(CPPFight_GameState the);

/* the current player index (ie you, if calling from inside CPPFight_Player_GetMove callback) */
int CPPFight_GameState_GetCurrentPlayer(CPPFight_GameState the);	
		
/* for iterating over players */
int CPPFight_GameState_GetNextPlayer(CPPFight_GameState the, int currentPlayerIndex);
int CPPFight_GameState_GetNextActivePlayer(CPPFight_GameState the, int currentPlayerIndex);

/* the number of players *still active* in game	*/
int CPPFight_GameState_GetActivePlayers(CPPFight_GameState the);

/* 1 if a player (by index) has any change left, 0 otherwise */
int CPPFight_GameState_IsPlayerActive(CPPFight_GameState the, int player );

/* get a copy of the change pool for a specific player (by index) */
void CPPFight_GameState_GetPlayerChange(CPPFight_GameState the, int player, CFIGHT_Change* result );
		
/* get a copy of the change pool currently available in the game (on the table) */
void CPPFight_GameState_GetGameChange(CPPFight_GameState the, CFIGHT_Change* result );

/* returns a new GameState object, representing the state of the game once a move has been played 
   the calling code is responable for deleting the returned object (see below)
*/
CPPFight_GameState CPPFight_GameState_PlayMove(CPPFight_GameState the, CFIGHT_Move move );

/* cleans up memory used in GameState objects returned by above function call */
void CPPFight_Delete_GameState( CPPFight_GameState the );

/*
  game, passed to AI get move callback, contains gamestate, plus player UIDs, and clocks.
*/

/* Get player UIDs by player index */
int CPPFight_Game_GetPlayerUID( CPPFight_Game the, int player ); 

/* get player time left by player index */
clock_t CPPFight_Game_GetPlayerTimeTaken( CPPFight_Game the, int player );

/* the game state of the current game - *DO NOT* Delete */
CPPFight_GameState CPPFight_Game_GetGameState( CPPFight_Game the );

#if defined(__cplusplus)
}//extern "C"
#endif 

#endif//CFIGHT_H_INCLUDED