#pragma once

#ifndef __SYNTACTIC_ANALYZER_GRAMMAR_BUILDER_H__
#define __SYNTACTIC_ANALYZER_GRAMMAR_BUILDER_H__

#include <vector>
#include "lib/hashtable.h"

namespace syntacticanalyzer {

	class Symbol;
	class TerminalSymbol;
	class NonterminalSymbol;
	class Production;
	class Grammar;

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

		virtual Symbol** symbols() = 0;
		virtual TerminalSymbol** terminalSymbols() = 0;
		virtual NonterminalSymbol** nonterminalSymbols() = 0;
		virtual Production** productions() = 0;

		virtual NonterminalSymbol* startSymbol() const = 0;
		virtual Production* initialProduction() const = 0;

		virtual unsigned int symbolCount() const = 0;
		virtual unsigned int terminalCount() const = 0;
		virtual unsigned int nonTerminalCount() const = 0;

		virtual Grammar* build() const = 0;
	};

	GrammarBuilder* createDefaultGrammarBuilder();

}

#endif