
//Last edit: 05/06/2017 13:44

#include "grammar.h"
#include <stdarg.h>

namespace syntacticanalyzer {

//
// Grammar
//

//Symbol Grammar::epsilon("EPSILON",TERMINAL);

	Grammar::Grammar()
		: m_startSym(NULL), m_eofSym(NULL), m_initialPro(NULL)
	{
		//m_eofSym is created here because it is going to be mapped to index 0
		m_eofSym = addTerminal("$eof");
	}

	Grammar::~Grammar()
	{
		unsigned int i;
		for(i = 0; i < m_symList.count(); ++i)
			delete m_symList[i];

		for(ProductionList::iterator proIt = m_proList.begin(); proIt != m_proList.end(); ++proIt)
			delete *proIt;
	}

	void Grammar::setStartSymbol(NonterminalSymbol *sym)
	{
		if(!sym)
			return;

		NonterminalSymbol *symStart = addNonTerminal("$start");
		if(sym == symStart)
			return;

		if(m_startSym) {
			if(m_startSym->proList().size() > 0)
				removeProduction(m_startSym->proList()[0]);
		}

		m_startSym = symStart;
		m_oldStartSym = sym;
		m_oldInitialProList = sym->proList();
		m_initialPro = addProduction(m_startSym, 2, sym, m_eofSym);
	}


	Symbol *Grammar::addSym(const char *name, SYMBOL_CLASS symClass)
	{
		Symbol *sym = findSymbol(name);
		if(sym) {
			//symbol already exists, check if the class is the same
			if(sym->symClass() != symClass)
				return NULL;
			return sym;
		}

		if(symClass == TERMINAL) {
			sym = new TerminalSymbol(name, m_symList.count());
			m_termSymList.append(sym);
		}
		else if(symClass == NONTERMINAL) {
			sym = new NonterminalSymbol(name, m_symList.count());
			m_nontermSymList.append(sym);
		}
		else {
			//Invalid Symbol Class
			return NULL;
		}

		m_symList.append(sym);
		m_symHT.insert((unsigned char*)name, strlen(name), sym);
		return sym;
	}

	Symbol *Grammar::findSymbol(const char *name)
	{
		/*
		u32 i;
		for(i = 0; i < m_symList.count(); ++i)
		{
			if(m_symList[i]->compareByName(name))
				return m_symList[i];
		}

		return NULL;
		*/

		return (Symbol*)m_symHT.find((unsigned char*)name, strlen(name));
	}

	Symbol *Grammar::symbol(unsigned int index) const
	{
		if(index >= m_symList.count())
			return NULL;
		return m_symList[index];
	}

	Production *Grammar::addProduction(NonterminalSymbol *lhs, unsigned int nrhs, ...)
	{
		if(!lhs)
			return NULL;

		Production *pro = new Production(lhs);

		va_list alist;
		va_start(alist, nrhs);

		for(unsigned int i = 0; i < nrhs; ++i) {
			pro->addRHS(va_arg(alist, Symbol*));
		}

		va_end(alist);

		pro->m_number = m_proList.size();
		m_proList.push_back(pro);
		return pro;
	}

	Production *Grammar::addProduction(NonterminalSymbol *lhs)
	{
		if(!lhs)
			return NULL;

		Production *pro = new Production(lhs);
		pro->m_number = m_proList.size();
		m_proList.push_back(pro);
		return pro;
	}

	Production *Grammar::addEBNFpro(const char *ebnf) {
		//to-do
		return NULL;
	}

	bool Grammar::removeSymbol(const char *symbol) {
		Symbol *sym = (Symbol*)m_symHT.remove((unsigned char*)symbol, strlen(symbol));
		if(sym) {
			delete sym;
			return true;
		}
		return false;
	}

	bool Grammar::removeSymbol(Symbol *sym) {
		if(sym != NULL) {
			if(m_symHT.remove((unsigned char*)sym->name(), strlen(sym->name())) != NULL) {
				if(sym->isNonTerminal()) {
					//todo: remove productions
				}
				delete sym;
				return true;
			}
		}
		return false;
	}

	bool Grammar::removeProduction(Production *pro) {
		if(pro != NULL) {
			for(ProductionList::iterator it = m_proList.begin(); it != m_proList.end(); ++it) {
				Production *toRemove = *it;
				if(toRemove == pro) {
					m_proList.erase(it);
					break;
				}
			}
			delete pro;
			return true;
		}
		return false;
	}

	Production *Grammar::production(unsigned int number) const
	{
		if(number >= m_proList.size())
			return NULL;
		return m_proList[number];
	}

}
