
//Last edit: 02/01/2020 19:21

#ifndef __SYNTACTIC_ANALYZER_SYMBOL_IMPL_H__
#define __SYNTACTIC_ANALYZER_SYMBOL_IMPL_H__

#include <vector>

#include "../../TerminalSymbol.h"
#include "../../NonterminalSymbol.h"

namespace syntacticanalyzer {

	class Symbol;
	class Production;

	typedef std::vector<Production*> ProductionList;

	class SymbolImpl : public Symbol {
		int m_index; //unique index
		const char *m_name; //symbol name (tag)

	protected:
		//only Grammar can create Symbol
		SymbolImpl(const char *name, int index = -1)
			: m_name(name), m_index(index) { }
		virtual ~SymbolImpl() { };

	public:
		friend class GrammarImpl;
		friend class GrammarBuilderImpl;

		virtual const char *name() const { return m_name; }
		virtual unsigned int index() const { return m_index; }
		virtual bool isTerminal() const { return false; }
		virtual bool isNonTerminal() const { return false; }

	};

	class TerminalSymbolImpl : public TerminalSymbol, public SymbolImpl {
		friend class GrammarImpl;
		friend class GrammarBuilderImpl;

		TerminalSymbolImpl(const char* name, int id)
			: SymbolImpl(name, id) {
		}

	public:
		virtual const char* name() const { return SymbolImpl::name(); }
		virtual unsigned int index() const { return SymbolImpl::index(); }
		virtual bool isTerminal() const { return true; }
		virtual bool isNonTerminal() const { return false; }
	};

	class NonterminalSymbolImpl : public NonterminalSymbol, public SymbolImpl {
		friend class GrammarImpl;
		friend class GrammarBuilderImpl;

		std::vector<Production*> m_proList; //productions with this symbol as lhs

		NonterminalSymbolImpl(const char *name, int id)
			: SymbolImpl(name, id) {
		}

		~NonterminalSymbolImpl() {
			/* who calls delete on me is responsible to delete and remove productions
			for(ProductionList::iterator it = m_proList.begin(); it != m_proList.end(); ++it) {
				delete *it;
			}
			*/
		};

		void addPro(Production &pro) { m_proList.push_back(&pro); }
		void removePro(Production &pro) {
			for(ProductionList::iterator it = m_proList.begin(); it != m_proList.end(); ++it) {
				if(*it == &pro) {
					m_proList.erase(it);
					return;
				}
			}
		}

	public:

		virtual const char* name() const { return SymbolImpl::name(); }
		virtual unsigned int index() const { return SymbolImpl::index(); }
		virtual bool isTerminal() const { return false; }
		virtual bool isNonTerminal() const { return true; }
		virtual Production** associatedProductions() const { return (Production **)&m_proList[0]; }
		virtual unsigned int associatedProductionCount() const { return m_proList.size(); }
	};

}

#endif