
//Last edit: 31/12/2019 20:41

#ifndef __SYNTACTIC_ANALYZER_GRAMMAR_IMPL_H__
#define __SYNTACTIC_ANALYZER_GRAMMAR_IMPL_H__

#include <vector>

#include <Grammar.h>
#include "../lib/hashtable.h"
#include "symbol.h"
#include "production.h"

namespace syntacticanalyzer {

	class GrammarImpl : public Grammar
	{
		friend class GrammarAnalyzer;

		std::vector<Symbol*> m_symList;
		ProductionList m_proList;

		//
		std::vector<TerminalSymbol*> m_termSymList;
		std::vector<NonterminalSymbol*> m_nontermSymList;
		mutable HashTable m_symHT;

		//
		NonterminalSymbol *m_startSym;
		Production* m_initialPro;

		//
		NonterminalSymbol* m_privateStartSym; //the start sym for the "augmented grammar"
		TerminalSymbol* m_eofSym;

		TerminalSymbol* addTerminal(const char* name);
		NonterminalSymbol* addNonterminal(const char* name);
		Production* addProduction(NonterminalSymbol* lhs, std::vector<Symbol*> rhs, SemanticAction action);

	public:
	//	static Symbol epsilon; //empty string symbol

		GrammarImpl(std::vector<Symbol *> symbolList, std::vector<Production *> productionList, NonterminalSymbol *startSymbol);
		~GrammarImpl();

		virtual Symbol *symbol(const char *name) const;
		virtual Symbol *symbol(unsigned int index) const;

		virtual Symbol** symbols() { return &m_symList[0]; }
		virtual TerminalSymbol** terminals() { return &m_termSymList[0]; }
		virtual NonterminalSymbol** nonterminals() { return &m_nontermSymList[0]; }
		virtual Production** productions() { return &m_proList[0]; }

		virtual Production* production(unsigned int number) const;

		virtual NonterminalSymbol* startSymbol() const { return m_startSym; }
		virtual Production* initialProduction() const { return m_initialPro; }

		virtual unsigned int symbolCount() const { return m_symList.size(); }
		virtual unsigned int terminalCount() const { return m_termSymList.size(); }
		virtual unsigned int nonterminalCount() const { return m_nontermSymList.size(); }
		virtual unsigned int productionCount() const { return m_proList.size(); }
	};

}

#endif