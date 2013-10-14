/*
	This file should list all the player AIs using C linkage,
	to be included in a tournement

	Call create functions ([AUTHOR]_CFIGHT_Create_[PLAYER_NAME]) inside CFIGHT_CreateAllPlayers
*/

#if defined(__cplusplus)
extern "C" {
#endif

int ExampleTeam_CFIGHT_Create_CPlayer();

void CFIGHT_CreateAllPlayers()
{
	ExampleTeam_CFIGHT_Create_CPlayer();
}

#if defined(__cplusplus)
}//extern "C"
#endif 