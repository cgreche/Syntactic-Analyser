
//Last edit: 03/01/2020 18:31

#ifndef __SYNTACTIC_ANALYZER_PRODUCTION_H__
#define __SYNTACTIC_ANALYZER_PRODUCTION_H__

#include "Symbol.h"

namespace syntacticanalyzer {

	class Token;
	class ParsingState;

	class SemanticValue {
	public:
		typedef union {
			char cval;
			short sval;
			long lval;
			int ival;
			char* strval;
			unsigned char ucval;
			unsigned short usval;
			unsigned long ulval;
			unsigned int uival;
			bool bval;
			float fval;
			double dval;
			void* pval;
		} Value;

		virtual Token* token() const = 0;
		virtual Value value() const = 0;
		virtual void setValue(Value value) = 0;

	};

	typedef void(*SemanticAction)(ParsingState& state, SemanticValue& ret);
#define SEMANTIC_ACTION(x) void x(ParsingState& state, SemanticValue& ret)
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