
//Last edit: 26/06/2017 03:03

#ifndef __GRAMMARANALYZER_H__
#define __GRAMMARANALYZER_H__

#include "grammar.h"
#include "state.h"
#include "defaultlexer.h"

namespace syntacticanalyzer {

	typedef signed char s8;
	typedef unsigned char u8;
	typedef signed short s16;
	typedef unsigned short u16;
	typedef signed long s32;
	typedef unsigned long u32;
	typedef unsigned int uint;

	class Symbol;
	class SymbolList;
	class Production;

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

#define SARG_(x) state->semanticStack[state->semanticArgsIndex + x]
#define SARG_COUNT (state->semanticStack.size() - state->semanticArgsIndex)
#define SARG_TOK(x) (SARG_(x).tok)
#define SARG_TEXT(x) (SARG_(x).text)
#define SARG(x,t) ((t)SARG_(x).value.pval)
#define SARG_CVAL(x) (SARG_(x).value.cval)
#define SARG_SVAL(x) (SARG_(x).value.sval)
#define SARG_LVAL(x) (SARG_(x).value.lval)
#define SARG_UCVAL(x) (SARG_(x).value.ucval)
#define SARG_USVAL(x) (SARG_(x).value.usval)
#define SARG_ULVAL(x) (SARG_(x).value.ulval)
#define SARG_IVAL(x) (SARG_(x).value.ival)
#define SARG_UVAL(x) (SARG_(x).value.uval)
#define SARG_PVAL(x) (SARG_(x).value.pval)
#define SARG_FVAL(x) (SARG_(x).value.fval)
#define SARG_DVAL(x) (SARG_(x).value.dval)
#define SRET(x) ret.value.pval = (void*)x
#define SRET_CVAL(x) (ret.value.cval = x)
#define SRET_SVAL(x) (ret.value.sval = x)
#define SRET_LVAL(x) (ret.value.lval = x)
#define SRET_UCVAL(x) (ret.value.ucval = x)
#define SRET_USVAL(x) (ret.value.usval = x)
#define SRET_ULVAL(x) (ret.value.ulval = x)
#define SRET_IVAL(x) (ret.value.ival = x)
#define SRET_UVAL(x) (ret.value.uval = x)
#define SRET_PVAL(x) (ret.value.pval = x)
#define SRET_FVAL(x) (ret.value.fval = x)
#define SRET_DVAL(x) (ret.value.dval = x)

	typedef std::vector<unsigned int> StateStack;
	typedef std::vector<Token> SemanticStack;

	struct ParsingState
	{
		GrammarAnalyzer &analyzer;
		int currentState;
		StateStack stateStack;
		SemanticStack semanticStack;
		unsigned int semanticArgsIndex;

		ParsingState(GrammarAnalyzer &analyzer)
			: analyzer(analyzer), currentState(NULL) {
		}
	};

	class GrammarAnalyzer
	{
		Grammar *m_grammar;

		std::vector<SymbolList> m_firstSets; //all grammar symbols first set
		std::vector<SymbolList> m_followSets; //all grammar symbols follow set
		std::vector<bool> m_nullable; //all grammar symbols, nullable?
		std::vector<bool> m_beingProcessed;

		//LR states
		std::vector<State*> m_states;

		u32 m_analysisState; //0: nothing, 1: lr applied, 2: parsing
		u32 m_analysisMethod;

		void _clean(); //reset parsing info (useful if we want to change the Grammar to be parsed)

		//
		ParsingState *m_parsingState;
		int **m_parsingTable;

		//Tokenization
		Lexer *m_lexer;
		Token m_curToken;

	private:
		State *addState(Symbol* acessingSymbol, Itemset &itemset);
		State *findState(Itemset &itemset);

		bool parse(std::ostream *stream); //main parsing method
		int getNextToken();

		bool _isnullable(Symbol *sym);
		void _fillFirst(Symbol *sym);

	public:
		GrammarAnalyzer(Grammar &grammar);
		~GrammarAnalyzer();

		Grammar *grammar() const { return m_grammar; }

		void computeNullable();
		void computeFirst();
		void computeFollow();
		SymbolList& first(u32 symbolIndex) { return m_firstSets[symbolIndex]; }
		SymbolList& follow(u32 symbolIndex) { return m_followSets[symbolIndex]; }
		bool isNullable(u32 symbolIndex) { return m_nullable[symbolIndex]; }

		void lr0(std::ostream *stream);
		void lalr(std::ostream *stream);
		void resolveConflicts();
		void generateParsingTable();

		unsigned int stateCount() const { return m_states.size(); }
		State *state(unsigned int number) const { return m_states[number]; }

		void dumpGrammar(std::ostream &stream);
		void dumpSymbols(std::ostream &stream);
		void dumpPros(std::ostream &stream);
		void dumpStates(std::ostream &stream);
		void dumpStateInfo(State &state, std::ostream &stream);
		void dumpFirstFollowNullable(std::ostream& stream);
		void dumpParsingTable(std::ostream &stream);

		//
		void setTokenizer(Lexer *lexer);
		bool parse(const char *input, std::ostream *stream); //parse an input file
	};

	/*
	class Language {
		Grammar *grammar;
		int **m_parsingTable;
	};

	class LanguageParser {
		Language *m_language;
		Lexer *m_lexer;

		Token m_curToken;

	public:
		LanguageParser(Language *language);
		void setTokenizer();
		bool parse(const char *input, std::ostream *stream); //parse an input file
		bool parse(unsigned int *input, std::ostream *stream); //parse a tokenized input file
	};
	*/

}

#endif