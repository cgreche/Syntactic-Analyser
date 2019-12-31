#pragma once

#include <vector>
#include "../../src/lib/hashtable.h"

namespace syntacticanalyzer {

	class Symbol;
	class TerminalSymbol;
	class NonterminalSymbol;
	class Production;
	class Grammar;

	typedef void* List;

	class ParsingState;
	class Token;
	typedef void(*SemanticAction)(ParsingState* state, Token& ret);

	class GrammarBuilder
	{
	public:
		virtual TerminalSymbol* addTerminal(const char* name) = 0;
		virtual NonterminalSymbol* addNonTerminal(const char* name) = 0;

		virtual void newProduction(NonterminalSymbol* lhs) = 0;
		virtual void addRHS(const char* name) = 0;
		virtual void addRHS(unsigned int index) = 0;
		virtual void setSemanticAction(SemanticAction action) = 0;
		virtual Production* addProduction() = 0;
		virtual Production* addProduction(const char* bnf) = 0;

		virtual	void setStartSymbol(NonterminalSymbol* sym) = 0;

		virtual Production* production(unsigned int number) const = 0;

		virtual bool removeSymbol(const char* symbol) = 0;
		virtual bool removeSymbol(Symbol* symbol) = 0;
		virtual bool removeProduction(Production* pro) = 0;

		virtual Symbol* symbol(const char* name) = 0;
		virtual Symbol* symbol(unsigned int index) const = 0;

		virtual List* symbols() = 0;
		virtual List* terminalSymbols() = 0;
		virtual List* nonterminalSymbols() = 0;
		virtual List* productions() = 0;

		virtual NonterminalSymbol* startSymbol() const = 0;
		virtual Production* initialProduction() const = 0;

		virtual unsigned int symbolCount() const = 0;
		virtual unsigned int terminalCount() const = 0;
		virtual unsigned int nonTerminalCount() const = 0;

		virtual Grammar* build() const = 0;
	};

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

	GrammarBuilder* createDefaultGrammarBuilder();

}