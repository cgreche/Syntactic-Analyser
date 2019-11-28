
//Last edit: 26/06/2017 02:04

#ifndef __PRODUCTION_H__
#define __PRODUCTION_H__

#include "symbol.h"

namespace syntacticanalyzer {

	struct ParsingState;
	struct Token;
	typedef void(*SemanticAction)(ParsingState *state, Token &ret);
#define SEMANTIC_ACTION(x) void x(ParsingState *state, Token &ret)
#define SEMANTIC_ACTION_C(x,y) SEMANTIC_ACTION(x) //commented

	class Production
	{
		unsigned int m_number;
		NonterminalSymbol *m_lhs; //head
		SymbolList m_rhs; //body

		//
		SemanticAction m_semanticAction;

		//only Grammar can create Production
		Production(NonterminalSymbol *lhs);
		~Production();

	public:
		friend class Grammar;
		void addRHS(Symbol *rhs);
		void setSemanticAction(SemanticAction action) { m_semanticAction = action; }
		NonterminalSymbol *lhs() const { return m_lhs; }
		Symbol *rhs(unsigned int n) const { if(n >= m_rhs.count()) return NULL; return m_rhs[n]; }
		unsigned int nrhs() const { return m_rhs.count(); }
		unsigned int number() const { return m_number; }

		SemanticAction semanticAction() const { return m_semanticAction; }
	};

}

#endif