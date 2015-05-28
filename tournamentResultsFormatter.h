#ifndef TOURNAMENT_RESULTS_FORMATTER_H_INCLUDED
#define TOURNAMENT_RESULTS_FORMATTER_H_INCLUDED

#include "cppFIGHT.h"

#include <vector>
#include <map>

// ABC
class TournamentResultsFormatter
{
	public:
		virtual void BeginTournament(const CPPFight::PlayerList& t) = 0;
		virtual void BeginRound(const CPPFight::PlayerList& r, int matches) = 0;
		virtual void AddMatchResult(
			const CPPFight::PlayerList& match,
			const std::vector<int>& games
		)=0;
		virtual void EndRound() = 0;
		virtual void EndTournament()=0;
};

class JsonResultsFormatter : public TournamentResultsFormatter
{
	public:
		JsonResultsFormatter();
		~JsonResultsFormatter();
		virtual void BeginTournament(const CPPFight::PlayerList& t);
		virtual void BeginRound(const CPPFight::PlayerList& r, int matches);
		virtual void AddMatchResult(
			const CPPFight::PlayerList& match,
			const std::vector<int>& games
		);
		virtual void EndRound();
		virtual void EndTournament();
		int m_tournament;
		int m_round;
		int m_match;
};

class PrintfEliminationFormatter : public TournamentResultsFormatter
{
	public:
		virtual void BeginTournament(const CPPFight::PlayerList& t);
		virtual void BeginRound(const CPPFight::PlayerList& r, int matches);
		virtual void AddMatchResult(
			const CPPFight::PlayerList& match,
			const std::vector<int>& games
		);
		virtual void EndRound();
		virtual void EndTournament();

	private:
		int round;
		int group;
};

class PrintfRoundRobinResultsFormatter : public TournamentResultsFormatter
{
	public:
		PrintfRoundRobinResultsFormatter();

		virtual void BeginTournament(const CPPFight::PlayerList& _tournament);

		virtual void AddMatchResult(
			const CPPFight::PlayerList& match,
			const std::vector<int>& games
		);

		virtual void EndTournament();

		//unused in this round-robin formatter
		virtual void BeginRound(const CPPFight::PlayerList& r, int matches);
		virtual void EndRound();

	private:
		CPPFight::PlayerList tournament;
		std::vector< std::vector<int> > xTable;
		std::map< int, int > rr_points_scores;	//total games won count
		std::map< int, int > rr_wld_scores;		//matches (N games) won/lost/draw

		int IndexOf( int id );

		int fi,fj;
		void FlushXTable(int i, int j, int s);
};


#endif