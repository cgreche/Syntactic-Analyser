
//Last edit: 02/01/2020 19:16

#ifndef __SYNTACTIC_ANALYZER_SYMBOL_H__
#define __SYNTACTIC_ANALYZER_SYMBOL_H__

#include <vector>

namespace syntacticanalyzer {

	class Symbol {
	public:
		virtual const char* name() const = 0;
		virtual unsigned int index() const = 0;
		virtual bool isTerminal() const = 0;
		virtual bool isNonTerminal() const = 0;
	};

	class TerminalSymbol : public Symbol {
	};

	class NonterminalSymbol : public Symbol {
	public:
		virtual Production** associatedProductions() const = 0;
		virtual unsigned int associatedProductionCount() const = 0;
	};
}

#endif