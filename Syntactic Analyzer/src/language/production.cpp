
//Last edit: 31/05/2017 16:38

#include "production.h"

namespace syntacticanalyzer {

	Production::Production(NonterminalSymbol *lhs)
		: m_semanticAction(NULL)
	{
		m_lhs = lhs;
		lhs->addPro(*this);
	}

	Production::~Production()
	{
		m_lhs->removePro(*this);
	}

	void Production::addRHS(Symbol *rhs)
	{
		if(rhs)
			m_rhs.append(rhs);
	}

}