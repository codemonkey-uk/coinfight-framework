
#include "tournamentResultsFormatter.h"
#include <cassert>

// JsonResultsFormatter

JsonResultsFormatter::JsonResultsFormatter()
{
	m_tournament=0;
}

JsonResultsFormatter::~JsonResultsFormatter()
{
	if (m_tournament>0) printf("]\n");
}

void JsonResultsFormatter::BeginTournament(const CPPFight::PlayerList& t)
{
	if (m_tournament>0) printf(",");
	else printf("[\n");
	printf("\{\n");
	printf("\"players\":[\n");
	for(int i=0;i!=t.size();++i)
	{
		if (i>0)  printf(",");
		printf("{\"uid\":%i,\"title\":\"%s\",\"author\":\"%s\"}",
			t[i]->GetUID(),
			t[i]->GetTitle().c_str(),
			t[i]->GetAuthor().c_str());
	}
	printf("],");
	printf("\"rounds\":[\n");
	m_round=0;
}

void JsonResultsFormatter::BeginRound(const CPPFight::PlayerList& r, int matches)
{
	if (m_round>0) printf(",");
	printf("[");
	m_match=0;
}

void JsonResultsFormatter::AddMatchResult(
	const CPPFight::PlayerList& match,
	const std::vector<int>& games
)
{
	if (m_match>0) printf(",");
	printf("[");
	for(int i=0;i!=match.size();++i)
	{
		if (i>0) printf(",");
		printf("{\"%i\":%i}", match[i]->GetUID(), games[i]);
	}
	printf("]");
	m_match++;
}

void JsonResultsFormatter::EndRound()
{
	m_round++;
	printf("]");
}

void JsonResultsFormatter::EndTournament()
{
	printf("]}\n");
	m_tournament++;
}

// PrintfEliminationFormatter

void PrintfEliminationFormatter::BeginTournament(const CPPFight::PlayerList& t)
{
	printf("\nMulitiplayer Elimination Tournament\n");
	round=1;
}

void PrintfEliminationFormatter::BeginRound(const CPPFight::PlayerList& r, int matches)
{
	printf(" Round %i - %i players, %i groups\n",
		round,
		static_cast<int>(r.size()),
		matches);
	group=1;
}

void PrintfEliminationFormatter::AddMatchResult(
	const CPPFight::PlayerList& match,
	const std::vector<int>& games
)
{
	printf("  Group %i, %i players.  Scores:\n",
		group,
		static_cast<int>(match.size()) );

	for(unsigned int i=0;i<match.size();++i){
		printf("    %i - %s by %s\n",
			games[ i ],
			match[i]->GetTitle().c_str(),
			match[i]->GetAuthor().c_str());
	}

	group++;
}

void PrintfEliminationFormatter::EndRound()
{
	round++;
}

void PrintfEliminationFormatter::EndTournament()
{
	// no-op
}

// PrintfRoundRobinResultsFormatter

// comparison functor for sorting players based on score
namespace
{
	class CompScores
	{
		public:
			CompScores(std::map< int, int >& scores)
				: myScores(scores)
			{
			}
			bool operator()(CPPFight::Player* a, CPPFight::Player* b)
			{
				return myScores[a->GetUID()] > myScores[b->GetUID()];
			}
		private:
			std::map< int, int >& myScores;
	};
}

PrintfRoundRobinResultsFormatter::PrintfRoundRobinResultsFormatter()
{
	fi=0;
	fj=0;
}

void PrintfRoundRobinResultsFormatter::BeginTournament(const CPPFight::PlayerList& _tournament)
{
	tournament = _tournament;
	xTable.resize( tournament.size() );

	printf(" Key:\n");
	for(int i=0;i<tournament.size();++i){

		xTable[i].resize( tournament.size() );
		for(int j=0;j<tournament.size();++j)
			xTable[i][j]=-1;

		printf("  %2i)\t%s by %s\n",
			tournament[i]->GetUID(),
			tournament[i]->GetTitle().c_str(),
			tournament[i]->GetAuthor().c_str());
	}

	printf(" Cross Table (rows=player 1, columns=player 2, table shows player 1 wins):\n\t");
	CPPFight::PlayerList::const_iterator k;
	for(k=tournament.begin();k!=tournament.end();++k){
		printf("%2i\t", (*k)->GetUID() );
	}
	printf("\n\t");
	for(k=tournament.begin();k!=tournament.end();++k){
		printf("--\t" );
	}
	printf("\n");
}

void PrintfRoundRobinResultsFormatter::AddMatchResult(
	const CPPFight::PlayerList& match,
	const std::vector<int>& games
)
{
	assert( games.size()==2 );

	const int i0=IndexOf(match[0]->GetUID());
	const int i1=IndexOf(match[1]->GetUID());
	FlushXTable(i0,i1,games[0]);

	rr_points_scores[ match[0]->GetUID() ] += games[0];
	rr_points_scores[ match[1]->GetUID() ] += games[1];

	//transfer match score to win/lose/draw score
	if (games[0] > games[1]){
		rr_wld_scores[match[0]->GetUID()]++;
	}
	else if(games[0] < games[1]){
		rr_wld_scores[match[1]->GetUID()]++;
	}
}

void PrintfRoundRobinResultsFormatter::EndTournament()
{
	//sort the player list on points score
	CPPFight::PlayerList tournament_sorted(tournament);
	std::sort( tournament_sorted.begin(), tournament_sorted.end(), CompScores(rr_points_scores) );

	printf(" Totals:\n  Games:\n");

	for(int i=0;i<tournament_sorted.size();++i){
		printf("%6i - %s by %s\n",
			rr_points_scores[ tournament_sorted[i]->GetUID() ],
			tournament_sorted[i]->GetTitle().c_str(),
			tournament_sorted[i]->GetAuthor().c_str());
	}

	//sort the player list on score
	std::sort( tournament_sorted.begin(), tournament_sorted.end(), CompScores(rr_wld_scores) );
	printf("  Matches:\n");

	for(int i=0;i<tournament_sorted.size();++i){
		printf("%6i - %s by %s\n",
			rr_wld_scores[ tournament_sorted[i]->GetUID() ],
			tournament_sorted[i]->GetTitle().c_str(),
			tournament_sorted[i]->GetAuthor().c_str());
	}
}

//unused in this round-robin formatter
void PrintfRoundRobinResultsFormatter::BeginRound(const CPPFight::PlayerList& r, int matches)
{
	// no-op
}

void PrintfRoundRobinResultsFormatter::EndRound()
{
	// no-op
}


int PrintfRoundRobinResultsFormatter::IndexOf( int id ) 
{
	for (int i=0;i!=tournament.size();++i)
		if (tournament[i]->GetUID()==id) return i;
	return -1;
}


void PrintfRoundRobinResultsFormatter::FlushXTable(int i,int j,int s)
{
	xTable[i][j] = s;

	while(xTable[fi][fj]!=-1 || fi==fj)
	{
		if (fj==0) printf("  %2i)\t", tournament[fi]->GetUID() );
		if (fi==fj) printf( "--\t" );
		else printf("%3i\t", xTable[fi][fj] );

		fj++;
		if (fj>=tournament.size())
		{
			fj=0;
			fi++;
			printf("\n");
			if (fi>=tournament.size())
				break;
		}
	}
}

