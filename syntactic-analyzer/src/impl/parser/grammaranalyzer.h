
//Last edit: 31/12/2019 20:19

#ifndef __SYNATCTIC_ANALYZER_GRAMMAR_ANALYZER_H__
#define __SYNTACTIC_ANALYZER_GRAMMAR_ANALYZER_H__

#include "../language/grammar.h"
#include "state.h"
#include "../lexer/defaultlexer.h"

namespace syntacticanalyzer {

	class Symbol;
	class Production;

	class Language;

#define ACTION_ERROR 0
#define ACTION_SHIFT 1
#define ACTION_REDUCE 2
#define ACTION_ACCEPT 3
#define MAKE_SHIFT_ACTION(targetStateIndex) ((ACTION_SHIFT << 0x10) | (targetStateIndex & 0xffff))
#define MAKE_REDUCE_ACTION(targetRuleIndex) ((ACTION_REDUCE << 0x10) | (targetRuleIndex & 0xffff))
#define MAKE_ACCEPT_ACTION(targetRuleIndex) ((ACTION_ACCEPT << 0x10) | (targetRuleIndex & 0xffff))
#define GET_ACTION(x) (x>>0x10)
#define GET_VALUE(x) (x&0xffff)

#define LR0 1<<0
#define SLR 1<<1
#define LALR 1<<2
#define LR1 1<<3
#define FIRST_COMPUTED 1<<8
#define FOLLOW_COMPUTED 1<<9

	class GrammarAnalyzer
	{
		GrammarImpl *m_grammar;

		std::vector<std::vector<Symbol*>> m_firstSets; //all grammar symbols first set
		std::vector<std::vector<Symbol*>> m_followSets; //all grammar symbols follow set
		std::vector<bool> m_nullable; //all grammar symbols, nullable?
		std::vector<bool> m_beingProcessed;

		//LR states
		std::vector<State*> m_states;
		int **m_parsingTable;

		int m_analysisState; //0: nothing, 1: lr applied, 2: parsing
		int m_analysisMethod;

		void _clean(); //reset parsing info (useful if we want to change the Grammar to be parsed)

	private:
		State *addState(Symbol* acessingSymbol, Itemset &itemset);
		State *findState(Itemset &itemset);

		bool _isnullable(Symbol *sym);
		void _fillFirst(Symbol *sym);

	public:
		GrammarAnalyzer(GrammarImpl &grammar);
		~GrammarAnalyzer();

		Grammar *grammar() const { return m_grammar; }

		void computeNullable();
		void computeFirst();
		void computeFollow();
		std::vector<Symbol*>& first(unsigned int symbolIndex) { return m_firstSets[symbolIndex]; }
		std::vector<Symbol*>& follow(unsigned int symbolIndex) { return m_followSets[symbolIndex]; }
		bool isNullable(unsigned int symbolIndex) { return m_nullable[symbolIndex]; }

		void lr0(std::ostream *stream);
		void lalr(std::ostream *stream);
		void resolveConflicts();
		void generateParsingTable();

		unsigned int stateCount() const { return m_states.size(); }
		State *state(unsigned int number) const { return m_states[number]; }

		int **parsingTable() const { return m_parsingTable; }

		void dumpGrammar(std::ostream &stream);
		void dumpSymbols(std::ostream &stream);
		void dumpPros(std::ostream &stream);
		void dumpStates(std::ostream &stream);
		void dumpStateInfo(State &state, std::ostream &stream);
		void dumpFirstFollowNullable(std::ostream& stream);
		void dumpParsingTable(std::ostream &stream);
	};

}

#endif