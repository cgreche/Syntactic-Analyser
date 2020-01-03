
//Last edit: 29/12/2019 02:00

#ifndef __PRODUCTION_H__
#define __PRODUCTION_H__

#include "Symbol.h"

namespace syntacticanalyzer {

	class ParsingState;
	class Token;

	typedef void(*SemanticAction)(ParsingState* state, Token& ret);
#define SEMANTIC_ACTION(x) void x(ParsingState *state, Token &ret)
#define SEMANTIC_ACTION_C(x,y) SEMANTIC_ACTION(x) //commented

	class Production
	{
	public:
		virtual NonterminalSymbol* lhs() const = 0;
		virtual Symbol* rhs(unsigned int n) const = 0;
		virtual unsigned int rhsCount() const = 0;
		virtual unsigned int number() const = 0;

		virtual SemanticAction semanticAction() const = 0;
	};

}

#endif