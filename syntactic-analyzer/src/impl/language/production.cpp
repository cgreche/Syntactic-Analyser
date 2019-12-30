
//Last edit: 31/05/2017 16:38

#include "production.h"

namespace syntacticanalyzer {

	ProductionImpl::ProductionImpl(NonterminalSymbol *lhs, std::vector<Symbol*> symbolList, SemanticAction action)
	{
		m_lhs = lhs;
		m_rhs = symbolList;
		m_semanticAction = action;
	}

	ProductionImpl::~ProductionImpl()
	{
	}

}