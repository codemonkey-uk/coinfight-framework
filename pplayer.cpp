#include <sys/types.h>
#include <sys/wait.h>
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

        execvp(*command, command);
        fprintf(stderr, "Failed to launch '%s', error:\n\t",*command);
        perror("execvp");
        exit(1);
    }

	close(p_stdin[READ]);
    if (infp == NULL)
        close(p_stdin[WRITE]);
    else
        *infp = p_stdin[WRITE];

	close(p_stdout[WRITE]);
    if (outfp == NULL)
        close(p_stdout[READ]);
    else
        *outfp = p_stdout[READ];

    return pid;
}

POpenPlayer::POpenPlayer( 
	const std::vector<std::string>& commands,
	const std::string& title, 
	const std::string& author ) 
: Player(title, author)
, mCommands(new char*[commands.size()+1])
{
	for (int i=0;i!=commands.size();++i)
		mCommands[i]=strdup(commands[i].c_str());
	mCommands[commands.size()]=0;
}

POpenPlayer::~POpenPlayer()
{
	int n=0;
	while(mCommands[n])
	{
		free(mCommands[n]);
		++n;
	}
	delete[] mCommands;
}

class MessageException : public Exception 
{
	public:
	MessageException(const std::string& error) : mError(error) {}
	virtual std::string ToString() const { return mError; }
	std::string mError;
};
		
Move POpenPlayer::GetMove( const Game& theGame )
{
	int inFp;
	int outFp;
	pid_t pid = popen2(mCommands, &inFp, &outFp);
	if (pid<0) {
		throw MessageException("C call to 'fork' failed launching bot.");
	}
	
	FILE* pIn = fdopen(inFp, "w");
	FILE* pOut = fdopen(outFp, "r");
		
	// write game state to in
	Serialise(pIn, theGame);
	fclose(pIn);
	
	// wait for the child process to finish
	int status;
	if  (waitpid(pid, &status, 0)!=pid)
		throw MessageException("C call to 'waitpid' failed waiting for bot.");
			
	// read move from out
	Move result(PENNY);
	if (Serialise(pOut,&result)==false)
	{
		// TODO: rewrite (de)serialisation code so we can collect & report the output
		throw MessageException(std::string("Failed to deserialise response to "));
	}
	fclose(pOut);
	
	return result;	
}
