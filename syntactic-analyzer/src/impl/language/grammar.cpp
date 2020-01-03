
//Last edit: 02/01/2020 20:58

#include "grammar.h"
#include <stdarg.h>

namespace syntacticanalyzer {

//
// Grammar
//

//Symbol Grammar::epsilon("EPSILON",TERMINAL);

	TerminalSymbol* GrammarImpl::addTerminal(const char* name)
	{
		TerminalSymbol* sym = new TerminalSymbolImpl(name, m_symList.size());
		m_termSymList.push_back(sym);
		m_symList.push_back(sym);
		m_symHT.insert((unsigned char*)name, strlen(name), sym);
		return sym;
	}

	NonterminalSymbol* GrammarImpl::addNonterminal(const char* name)
	{
		NonterminalSymbol* sym = new NonterminalSymbolImpl(name, m_symList.size());
		m_nontermSymList.push_back(sym);
		m_symList.push_back(sym);
		m_symHT.insert((unsigned char*)name, strlen(name), sym);
		return sym;
	}

	Production* GrammarImpl::addProduction(NonterminalSymbol* lhs, std::vector<Symbol*> rhs, SemanticAction action)
	{
		ProductionImpl* pro = new ProductionImpl(lhs,rhs,action);
		pro->m_number = m_proList.size();
		m_proList.push_back(pro);
		return pro;
	}

	GrammarImpl::GrammarImpl(std::vector<Symbol*> symbolList, std::vector<Production *> productionList, NonterminalSymbol *startSymbol)
	{
		//m_eofSym is created here because it is going to be mapped to index 0
		m_eofSym = addTerminal("$eof");

		for (std::vector<Symbol*>::iterator it = symbolList.begin(); it != symbolList.end(); ++it) {
			Symbol* sym = *it;
			if (m_symHT.insert((unsigned char*)sym->name(), ::strlen(sym->name()), sym) == NULL) {
				//new element
				m_symList.push_back(sym);
				if (sym->isTerminal())
					m_termSymList.push_back((TerminalSymbol*)sym);
				else
					m_nontermSymList.push_back((NonterminalSymbol*)sym);
			}
		}

		m_proList = productionList;
		m_startSym = startSymbol;
		m_initialPro = startSymbol->associatedProductions()[0];
		m_privateStartSym = addNonterminal("$start");
		std::vector<Symbol*> rhs = { m_startSym, m_eofSym };
		addProduction(m_privateStartSym, rhs, 0);
	}

	GrammarImpl::~GrammarImpl()
	{
		unsigned int i;
		for(i = 0; i < m_symList.size(); ++i)
			delete m_symList[i];

		for(ProductionList::iterator proIt = m_proList.begin(); proIt != m_proList.end(); ++proIt)
			delete *proIt;
	}

	Symbol * GrammarImpl::symbol(const char *name) const
	{
		return (Symbol*)m_symHT.find((unsigned char*)name, strlen(name));
	}

	Symbol * GrammarImpl::symbol(unsigned int index) const
	{
		if(index >= m_symList.size())
			return NULL;
		return m_symList[index];
	}

	Production * GrammarImpl::production(unsigned int number) const
	{
		if(number >= m_proList.size())
			return NULL;
		return m_proList[number];
	}

}
