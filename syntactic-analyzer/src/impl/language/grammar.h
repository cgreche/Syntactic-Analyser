
//Last edit: 29/12/2019 04:20

#ifndef __GRAMMAR_IMPL_H__
#define __GRAMMAR_IMPL_H__

#include <vector>

#include "../lib/hashtable.h"
#include "symbol.h"
#include "production.h"
#include "../Grammar.h"

namespace syntacticanalyzer {

	typedef std::vector<Symbol*> SymbolList;
	typedef std::vector<Production*> ProductionList;

	class GrammarImpl : public Grammar
	{
		SymbolList m_symList;
		ProductionList m_proList;

		//
		SymbolList m_termSymList;
		SymbolList m_nontermSymList;
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

		friend class GrammarAnalyzer;

		GrammarImpl(std::vector<Symbol *> symbolList, std::vector<Production *> productionList, NonterminalSymbol *startSymbol);
		~GrammarImpl();

		virtual Symbol *symbol(const char *name) const;
		virtual Symbol *symbol(unsigned int index) const;

		virtual SymbolList& symbolList() { return m_symList; }
		virtual SymbolList& terminalSymbols() { return m_termSymList; }
		virtual SymbolList& nonterminalSymbols() { return m_nontermSymList; }
		ProductionList& productionList() { return m_proList; }

		Production* production(unsigned int number) const;

		virtual NonterminalSymbol* startSymbol() const { return m_startSym; }
		virtual Production* initialProduction() const { return m_initialPro; }

		virtual unsigned int symbolCount() const { return m_symList.size(); }
		virtual unsigned int terminalCount() const { return m_termSymList.size(); }
		virtual unsigned int nonTerminalCount() const { return m_nontermSymList.size(); }
	};

}

#endif