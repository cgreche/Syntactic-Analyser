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

	class GrammarBuilder
	{
	public:
		virtual TerminalSymbol* addTerminal(const char* name) = 0;
		virtual NonterminalSymbol* addNonterminal(const char* name) = 0;

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

		virtual Symbol* symbol(const char* name) const = 0;
		virtual Symbol* symbol(unsigned int index) const = 0;

		virtual Symbol** symbols() const = 0;
		virtual TerminalSymbol** terminalSymbols() const = 0;
		virtual NonterminalSymbol** nonterminalSymbols() const = 0;
		virtual Production** productions() const = 0;

		virtual NonterminalSymbol* startSymbol() const = 0;

		virtual unsigned int symbolCount() const = 0;
		virtual unsigned int terminalCount() const = 0;
		virtual unsigned int nonTerminalCount() const = 0;

		virtual Grammar* build() = 0;
	};

	GrammarBuilder* createDefaultGrammarBuilder();

}

#endif