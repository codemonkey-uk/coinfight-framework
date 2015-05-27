#include "argumentProcessor.h"
#include <cstdlib>

int ParseCLInt(const char* str, int lower, int upper, const char* docstr)
{
	int result=atoi(str);
	if (result<lower || result>upper) {
		fprintf(stderr,"Illegal %s: %s (%i)\n",
			docstr,
			str,
			result);
		exit(-1);
	}
	return result;
}

void CommandLineSwitch::PrintDiagnostic(const std::vector<std::string>& arguments)
{
	fprintf(stderr, "Found %i arguments:\n\t", (int)arguments.size());
	for (std::vector<std::string>::const_iterator i = arguments.begin();
		i != arguments.end(); ++i)
	{
		fprintf(stderr, "'%s' ", i->c_str());
	}
	fprintf(stderr, "\n");
}

void SetGlobalInt::Consume(const std::vector<std::string>& arguments)
{
	if (arguments.size()!=1)
	{
		fprintf(stderr, "Error. Expected exactly 1 argument.\n");
		PrintDiagnostic(arguments);
		exit(-1);
	}

	*m_pOption = ParseCLInt(arguments.front().c_str(), m_Lower, m_Upper, m_ShortStr.c_str());
}
	
void SetGlobalOption::Consume(const std::vector<std::string>& arguments)
{
	if (arguments.size()!=1)
	{
		fprintf(stderr, "Error. Expected exactly 1 argument.\n");
		PrintDiagnostic(arguments);
		exit(-1);
	}

	*m_pOption = arguments.front();
}

void EnableGlobalSwitch::Consume(const std::vector<std::string>& arguments)
{
	if (arguments.empty()==false)
	{
		fprintf(stderr, "Error. Expected 0 arguments.\n");
		PrintDiagnostic(arguments);
		exit(-1);
	}

	*m_pOption = true;
}

void ProcessCommandLineArguments(int argc, char* argv[], 
	const std::map<char,CommandLineSwitch*>& commandMap)
{
	std::vector<std::string> arguments;
	CommandLineSwitch* command = 0;
	for (int i=1;i<argc;++i)
	{
		if (*argv[i]=='-')
		{
			if (command)
			{
				command->Consume(arguments);
				command = 0;
				arguments.resize(0);
			}

			char mode = *(argv[i]+1);
			if (mode)
			{
				if (*(argv[i]+2)!=0) {
					arguments.push_back( std::string(argv[i]+2) );
				}
			}
			std::map<char,CommandLineSwitch*>::const_iterator itr = commandMap.find(mode);
			command = itr!=commandMap.end() ? itr->second : 0;
			if (!command)
			{
				fprintf(stderr,"Unrecognised command '%c'\n", mode);
				exit(-1);
			}
		}
		else if (command)
		{
			arguments.push_back( argv[i] );
		}
		else
		{
			// unexpected
			fprintf(stderr,"Unexpected error processing command line argument %s: ", argv[i] );
			exit(-1);
		}
	}

	if (command)
		command->Consume(arguments);
}
