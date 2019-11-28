
//Last edit: 17/02/2018 21:59

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <vector>

namespace syntacticanalyzer {

	class Symbol;
	class SymbolList;
	class Production;

	typedef std::vector<Production*> ProductionList;

	typedef enum
	{
		TERMINAL,
		NONTERMINAL,
		SPECIAL
	} SYMBOL_CLASS;


	class Symbol {
		int m_index; //unique index
		const char *m_name; //symbol name (tag)

	protected:
		//only Grammar can create Symbol
		Symbol(const char *name, int index = -1)
			: m_name(name), m_index(index) { }
		virtual ~Symbol() { };

	public:
		friend class Grammar;

		virtual SYMBOL_CLASS symClass() const = 0; //terminal, nonterminal

		const char *name() const { return m_name; }
		inline unsigned int index() const { return m_index; }
		inline bool isTerminal() const { return symClass() == TERMINAL; }
		inline bool isNonTerminal() const { return symClass() == NONTERMINAL; }

		bool compareByName(const char *name) {
			return strcmp(m_name, name) == 0;
		}
	};

	class TerminalSymbol : public Symbol {
		friend class Grammar;
		TerminalSymbol(const char *name, int id)
			: Symbol(name, id) {
		}

	public:
		virtual SYMBOL_CLASS symClass() const { return TERMINAL; }
	};

	class NonterminalSymbol : public Symbol {
		ProductionList m_proList; //productions with this symbol as lhs
		NonterminalSymbol(const char *name, int id)
			: Symbol(name, id) {
		}
		~NonterminalSymbol() {
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
		friend class Grammar;
		friend class Production;

		virtual SYMBOL_CLASS symClass() const { return NONTERMINAL; }

		inline ProductionList& proList() { return m_proList; }
	};


	class SymbolList {
		std::vector<Symbol*> m_symbols;

	public:
		bool append(Symbol* sym);
		unsigned int merge(const SymbolList& symList);
		bool contains(Symbol* sym);
		void clear() { if(!m_symbols.empty()) m_symbols.clear(); }

		unsigned int count() const { return m_symbols.size(); }

		Symbol* operator[](int pos) const { return m_symbols[pos]; }
	};

}

#endif