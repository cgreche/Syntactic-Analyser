
//Created in: 29/12/2019 03:19
//Last edit: 29/12/2019 03:19

#ifndef __SYNTACTIC_ANALYZER_GRAMMAR_H___
#define __SYNTACTIC_ANALYZER_GRAMMAR_H___

#include <vector>

namespace syntacticanalyzer {

	typedef void* List;

	class Symbol;
	class TerminalSymbol;
	class NonterminalSymbol;
	class Production;

	class Grammar
	{
	public:

		/*
		virtual List* symbols() = 0;
		virtual List* terminalSymbols() = 0;
		virtual List* nonterminalSymbols() = 0;
		virtual List* productions() = 0;
		*/

		virtual Symbol* symbol(const char* name) const = 0;
		virtual Symbol* symbol(unsigned int index) const = 0;

		virtual NonterminalSymbol* startSymbol() const = 0;
		virtual Production* initialProduction() const = 0;

		virtual unsigned int symbolCount() const = 0;
		virtual unsigned int terminalCount() const = 0;
		virtual unsigned int nonTerminalCount() const = 0;
	};

}

#endif