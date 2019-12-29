
//Last edit: 26/06/2017 04:18

#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__

#include <vector>

#include "../lib/hashtable.h"
#include "symbol.h"
#include "production.h"

namespace syntacticanalyzer {

	typedef std::vector<Production*> ProductionList;

	class Grammar
	{
		SymbolList m_symList;
		ProductionList m_proList;

		//
		SymbolList m_termSymList;
		SymbolList m_nontermSymList;
		HashTable m_symHT;

		//
		NonterminalSymbol *m_oldStartSym; //the symbol that was passed as argument to setStartSymbol
		ProductionList m_oldInitialProList; //productions associated to m_oldStartSymbol
		NonterminalSymbol *m_startSym; //set after setStartSymbol()
		TerminalSymbol *m_eofSym;
		Production* m_initialPro; //set after setStartSymbol()

		//
		Symbol *addSym(const char *name, SYMBOL_CLASS symclass = NONTERMINAL);

	public:
	//	static Symbol epsilon; //empty string symbol

		friend class GrammarAnalyzer;

		Grammar();
		~Grammar();

		void setStartSymbol(NonterminalSymbol *sym);
		TerminalSymbol *addTerminal(const char *name) { return (TerminalSymbol*)addSym(name, TERMINAL); }
		NonterminalSymbol *addNonTerminal(const char *name) { return (NonterminalSymbol*)addSym(name, NONTERMINAL); }
		Symbol *findSymbol(const char *name);
		Symbol *symbol(unsigned int index) const;
		Production *addProduction(NonterminalSymbol *lhs, unsigned int nrhs, ...);
		Production *addProduction(NonterminalSymbol *lhs);
		Production *addEBNFpro(const char *ebnf);
		Production *production(unsigned int number) const;

		bool removeSymbol(const char *symbol);
		bool removeSymbol(Symbol *symbol);
		bool removeProduction(Production *pro);

		SymbolList& symbolList() { return m_symList; }
		SymbolList& terminalSymbols() { return m_termSymList; }
		SymbolList& nonterminalSymbols() { return m_nontermSymList; }
		ProductionList& productionList() { return m_proList; }

		Symbol* startSymbol() const { return m_startSym; }
		Production* initialProduction() const { return m_initialPro; }

		unsigned int symbolCount() const { return m_symList.count(); }
		unsigned int terminalCount() const { return m_termSymList.count(); }
		unsigned int nonTerminalCount() const { return m_nontermSymList.count(); }
	};

}

#endif