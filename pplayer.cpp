#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pplayer.h"
#include <string>
#include <stdio.h>

using namespace CPPFight;

#define READ 0
#define WRITE 1

pid_t
//popen2(const char *command, int *infp, int *outfp)
popen2(char **command, int *infp, int *outfp)
{
    int p_stdin[2], p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
        return -1;

    pid = fork();

    if (pid < 0)
        return pid;
    else if (pid == 0)
    {
        close(p_stdin[WRITE]);
        dup2(p_stdin[READ], READ);
        close(p_stdout[READ]);
        dup2(p_stdout[WRITE], WRITE);

        //execl("/bin/sh", "sh", "-c", command, NULL);
        execvp(*command, command);
        perror("execl");
        exit(1);
    }

    if (infp == NULL)
        close(p_stdin[WRITE]);
    else
        *infp = p_stdin[WRITE];

    if (outfp == NULL)
        close(p_stdout[READ]);
    else
        *outfp = p_stdout[READ];

    return pid;
}

POpenPlayer::POpenPlayer( const std::string& command,
	const std::string& title, const std::string& author ) 
: Player(title, author), mCommand(command)
{
	myPid = 0;//popen2(command.c_str(), &myInFp, &myOutFp);
	m_pIn = 0;//fdopen(myInFp, "w");
	m_pOut =0;//fdopen(myOutFp, "r");		
}

Move POpenPlayer::GetMove( const Game& theGame )
{
	//if (myPid==0)
	//{
		//myPid = popen2(mCommand.c_str(), &myInFp, &myOutFp);
		char *command[] = {"./a.out", "AllLow", NULL};
		myPid = popen2(command, &myInFp, &myOutFp);
		m_pIn = fdopen(myInFp, "w");
		m_pOut = fdopen(myOutFp, "r");
	//}
	
	// write game state to in
	fprintf(stderr,"Writing game state to child process...");
	Serialise(m_pIn, theGame);
	
	fflush(m_pIn);
	fclose(m_pIn);
	
	// read move from out
	fprintf(stderr,"\nWaiting for response...");	
	Move result(PENNY);
	if (Serialise(m_pOut,&result)==false)
		throw Exception();
	
	fflush(m_pOut);
	fclose(m_pOut);
	
	fprintf(stderr,"\nFinished move!\n");	
	return result;	
}
