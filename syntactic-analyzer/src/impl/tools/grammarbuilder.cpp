
#include "../language/grammar.h"
#include "../language/symbol.h"
#include "grammarbuilder.h"

#include <cstdarg>

namespace syntacticanalyzer {

	void GrammarBuilderImpl::linkSymbolToPro(NonterminalSymbol* symbol, Production* pro) {
		((NonterminalSymbolImpl*)symbol)->addPro(*pro);
	}

	TerminalSymbol* GrammarBuilderImpl::addTerminal(const char* name) {
		Symbol* sym = symbol(name);
		if (sym) {
			if (sym->isTerminal())
				return (TerminalSymbol*)sym;
			else {
				//TODO: Error: trying to add a terminal symbol added previously as nonterminal
				return NULL;
			}
		}

		TerminalSymbol* terminalSymbol = new TerminalSymbolImpl(name,m_symList.size()+1); //id 0 is reserved
		m_symHT.insert((const unsigned char*)name, strlen(name), terminalSymbol);
		m_symList.push_back(terminalSymbol);
		m_termSymList.push_back(terminalSymbol);
		return terminalSymbol;
	}

	NonterminalSymbol* GrammarBuilderImpl::addNonterminal(const char* name) {
		Symbol* sym = symbol(name);
		if (sym) {
			if (sym->isNonterminal())
				return (NonterminalSymbol*)sym;
			else {
				//TODO: Error: trying to add a nont0erminal symbol added previously as terminal
				return NULL;
			}
		}

		NonterminalSymbol* terminalSymbol = new NonterminalSymbolImpl(name, m_symList.size() + 1); //id 0 is reserved
		m_symHT.insert((const unsigned char*)name, strlen(name), terminalSymbol);
		m_symList.push_back(terminalSymbol);
		m_nontermSymList.push_back(terminalSymbol);
		return terminalSymbol;
	}

	void GrammarBuilderImpl::newProduction(NonterminalSymbol* lhs) {
		if(!m_proRHS.empty())
			m_proRHS.clear();
		m_proLHS = lhs;
		m_proSemanticAction = 0;
	}

	void GrammarBuilderImpl::addRHS(const char* name) {
		Symbol* rhs = this->symbol(name);
		//TODO: check for null
		m_proRHS.push_back(rhs);
	}

	void GrammarBuilderImpl::addRHS(unsigned int index) {
		Symbol* rhs = this->m_symList[index];
		//TODO: check for valid index
		m_proRHS.push_back(rhs);
	}

	void GrammarBuilderImpl::setSemanticAction(SemanticAction semanticAction) {
		m_proSemanticAction = semanticAction;
	}

	Production* GrammarBuilderImpl::addProduction() {
		ProductionImpl* pro = new ProductionImpl(m_proLHS,m_proRHS,m_proSemanticAction);
		linkSymbolToPro(m_proLHS,pro);
		pro->m_number = m_proList.size();
		m_proList.push_back(pro);
		return pro;
	}

	Production* GrammarBuilderImpl::addProduction(const char* bnf) {
		//TODO:
		return NULL;
	}


	void GrammarBuilderImpl::setStartSymbol(NonterminalSymbol* sym) {
		m_startSym = sym;
	}

	bool GrammarBuilderImpl::removeSymbol(const char* symbol) {
		//TODO: remove from lists
		Symbol* sym = (Symbol*)m_symHT.remove((unsigned char*)symbol, strlen(symbol));
		if (sym) {
			delete sym;
			return true;
		}
		return false;
	}

	bool GrammarBuilderImpl::removeSymbol(Symbol* symbol) {
		//TODO: remove from lists
		if (symbol != NULL) {
			if (m_symHT.remove((unsigned char*)symbol->name(), strlen(symbol->name())) != NULL) {
				if (symbol->isNonterminal()) {
					//todo: remove productions
				}
				delete symbol;
				return true;
			}
		}
		return false;
	}

	bool GrammarBuilderImpl::removeProduction(Production* pro) {
		//TODO: remove from lists
		if (pro != NULL) {
			for (ProductionList::iterator it = m_proList.begin(); it != m_proList.end(); ++it) {
				Production* toRemove = *it;
				if (toRemove == pro) {
					m_proList.erase(it);
					break;
				}
			}
			delete pro;
			return true;
		}
		return false;
	}

	Production* GrammarBuilderImpl::production(unsigned int number) const {
		return m_proList[number];
	}

	Symbol* GrammarBuilderImpl::symbol(const char* name) const {
		return (Symbol*)m_symHT.find((unsigned char *)name, ::strlen(name));
	}

	Symbol* GrammarBuilderImpl::symbol(unsigned int index) const {
		return m_symList[index];
	}

	Symbol** GrammarBuilderImpl::symbols() const {
		return 0;
	}

	TerminalSymbol** GrammarBuilderImpl::terminalSymbols() const {
		return 0;
	}

	NonterminalSymbol** GrammarBuilderImpl::nonterminalSymbols() const {
		return 0;
	}

	Production** GrammarBuilderImpl::productions() const {
		return 0;
	}

	NonterminalSymbol* GrammarBuilderImpl::startSymbol() const {
		return m_startSym;
	}

	unsigned int GrammarBuilderImpl::symbolCount() const {
		return m_symList.size();
	}

	unsigned int GrammarBuilderImpl::terminalCount() const {
		return m_termSymList.size();
	}

	unsigned int GrammarBuilderImpl::nonTerminalCount() const {
		return m_nontermSymList.size();
	}

	Grammar* GrammarBuilderImpl::build() {
		return new GrammarImpl(m_symList,m_proList,m_startSym);
	}

	extern "C" GrammarBuilder* createDefaultGrammarBuilder() {
		return new GrammarBuilderImpl;
	}
}