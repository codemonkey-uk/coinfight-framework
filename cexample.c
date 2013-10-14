/*
   example player code for the C API for cppFIGHT v1.8 +
   by Thaddaeus Frogley
   Build, & link with 
     cppFIGHT.cpp, 
	 cexample.c, &
	 cplayers.c
   and run.
   
   See also cFIGHT.h
*/

#if defined(__cplusplus)
extern "C" {
#endif

#include "cFIGHT.h"
#include "memory.h"

/*
	Create function, by convention avoid link clashes by using the form:
	([AUTHOR]_CFIGHT_[PLAYER_NAME]_MoveFn)
*/

CFIGHT_Move ExampleTeam_CFIGHT_CPlayer_MoveFn(CPPFight_Player player, CPPFight_Game g)
{
	int current_player,i;
	CFIGHT_Coin lowest;

	CFIGHT_Change hand, pot;
	CFIGHT_Move move;
	
	/* get the gamestate from the game */
	CPPFight_GameState game;
	game = CPPFight_Game_GetGameState( g );

	/* fetch objects from the gamestate */
	current_player = CPPFight_GameState_GetCurrentPlayer( game );
	CPPFight_GameState_GetPlayerChange( game, current_player, &hand );
	CPPFight_GameState_GetGameChange( game, &pot );

	/* find lowest value coin in hand */
	lowest = CFIGHT_PENNY;	
	for (i = 0; i < CFIGHT_COIN_COUNT; ++i){		
		if (hand.change[i] > 0) {
			lowest = CFIGHT_Index2Coin( i );;
			break;
		}
	}

	move.play_coin = lowest;
	CFIGHT_MaximumChangeValue( &pot, lowest, &move.take_change );

	return move;
}

/*
	Create function, by convention avoid link clashes by using the form:
	([AUTHOR]_CFIGHT_Create_[PLAYER_NAME])
*/
int ExampleTeam_CFIGHT_Create_CPlayer()
{
	CPPFight_CPlayerCfg myPlayer;
	memset(&myPlayer,0, sizeof(CPPFight_CPlayerCfg));
	myPlayer.pGetMove = ExampleTeam_CFIGHT_CPlayer_MoveFn;
	CPPFight_New_Player("CPlayer", "Examples Team", myPlayer);
	return 0;
}

#if defined(__cplusplus)
}//extern "C"
#endif 
