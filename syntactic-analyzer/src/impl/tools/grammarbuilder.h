#pragma once

#include <vector>
#include "../GrammarBuilder.h"

namespace syntacticanalyzer {

	class GrammarBuilderImpl : public GrammarBuilder {
		std::vector<Symbol*> m_symList;
		std::vector<Production*> m_proList;

		//
		std::vector<Symbol*> m_termSymList;
		std::vector<Symbol*> m_nontermSymList;
		mutable HashTable m_symHT;

		//
		NonterminalSymbol* m_startSym; //set after setStartSymbol()

		//Production building context
		NonterminalSymbol* m_proLHS;
		std::vector<Symbol*> m_proRHS;
		SemanticAction m_proSemanticAction;

		//
		void linkSymblToPro(NonterminalSymbol* symbol, Production* pro);

	public:
		virtual TerminalSymbol* addTerminal(const char* name);
		virtual NonterminalSymbol* addNonTerminal(const char* name);

		virtual void newProduction(NonterminalSymbol* lhs);
		virtual void addRHS(Symbol* rhs);
		virtual void setSemanticAction(SemanticAction action);
		virtual Production* addProduction();
		virtual Production* addProduction(const char *bnf);

		virtual	void setStartSymbol(NonterminalSymbol* sym);

		virtual bool removeSymbol(const char* symbol);
		virtual bool removeSymbol(Symbol* symbol);
		virtual bool removeProduction(Production* pro);

		virtual Production* production(unsigned int number) const;

		virtual Symbol* symbol(const char* name) const;
		virtual Symbol* symbol(unsigned int index) const;

		virtual List* symbols();
		virtual List* terminalSymbols();
		virtual List* nonterminalSymbols();
		virtual List* productions();

		virtual NonterminalSymbol* startSymbol() const;

		virtual unsigned int symbolCount() const;
		virtual unsigned int terminalCount() const;
		virtual unsigned int nonTerminalCount() const;

		virtual Grammar* build();
	};

}