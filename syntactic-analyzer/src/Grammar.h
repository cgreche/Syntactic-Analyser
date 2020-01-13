
//Created in: 29/12/2019 03:19
//Last edit: 02/01/2020 19:13

#ifndef __SYNTACTIC_ANALYZER_GRAMMAR_H___
#define __SYNTACTIC_ANALYZER_GRAMMAR_H___

#include "Production.h"

namespace syntacticanalyzer {

	typedef void* List;

	class Symbol;
	class TerminalSymbol;
	class NonterminalSymbol;
	class Production;

	class Grammar
	{
	public:
		virtual Symbol** symbols() = 0;
		virtual TerminalSymbol** terminals() = 0;
		virtual NonterminalSymbol** nonterminals() = 0;
		virtual Production** productions() = 0;

		virtual Symbol* symbol(const char* name) const = 0;
		virtual Symbol* symbol(unsigned int index) const = 0;
		virtual Production* production(unsigned int number) const = 0;

		virtual NonterminalSymbol* startSymbol() const = 0;
		virtual Production* initialProduction() const = 0;

		virtual unsigned int symbolCount() const = 0;
		virtual unsigned int terminalCount() const = 0;
		virtual unsigned int nonterminalCount() const = 0;
		virtual unsigned int productionCount() const = 0;
	};

}

#endif