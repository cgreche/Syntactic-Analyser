//Created in: 29/12/2019 03:19
//Last edit: 02/01/2020 19:16

#ifndef __SYNTACTIC_ANALYZER_NONTERMINAL_SYMBOL_H__
#define __SYNTACTIC_ANALYZER_NONTERMINAL_SYMBOL_H__


#pragma once

#include "Symbol.h"

namespace syntacticanalyzer {

	class Production;

	class NonterminalSymbol : public Symbol {
	public:
		virtual Production** associatedProductions() const = 0;
		virtual unsigned int associatedProductionCount() const = 0;
	};

}

#endif