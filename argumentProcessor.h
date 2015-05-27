#ifndef ARGUMENTS_PROCESSOR_H_INCLUDED
#define ARGUMENTS_PROCESSOR_H_INCLUDED

#include <string>
#include <map>
#include <vector>

class CommandLineSwitch
{
	public:
		CommandLineSwitch(std::string descr)
			: mDescription (descr)
		{
		}
		virtual ~CommandLineSwitch() {};
		virtual void Consume(const std::vector<std::string>& arguments)=0;
		const std::string& Description() const { return mDescription; }
	private:
		std::string mDescription;
	protected:
		void PrintDiagnostic(const std::vector<std::string>& arguments);
};

class SetGlobalInt : public CommandLineSwitch
{
	public:
		SetGlobalInt(int* pOption, int lower, int upper, const std::string& descr, const std::string& short_descr)
			: CommandLineSwitch(descr)
			, m_pOption(pOption)
			, m_Lower(lower), m_Upper(upper)
			, m_ShortStr(short_descr)
		{
		}
		void Consume(const std::vector<std::string>& arguments);
	private:
		int* m_pOption;
		int m_Lower, m_Upper;
		std::string m_ShortStr;
};

class SetGlobalOption : public CommandLineSwitch
{
	public:
		SetGlobalOption(std::string* pOption, const std::string& descr)
			: CommandLineSwitch(descr)
			, m_pOption(pOption)
		{
		}
		void Consume(const std::vector<std::string>& arguments);
	private:
		std::string* m_pOption;
};

class EnableGlobalSwitch : public CommandLineSwitch
{
	public:
		EnableGlobalSwitch(bool* pOption, const std::string& descr)
			: CommandLineSwitch(descr)
			, m_pOption(pOption)
		{
		}
		void Consume(const std::vector<std::string>& arguments);
	private:
		bool* m_pOption;
};

void ProcessCommandLineArguments(int argc, char* argv[], 
	const std::map<char,CommandLineSwitch*>& commandMap);

#endif