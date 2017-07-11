
//Last edit: 31/05/2017 16:52

#include "grammar.h"
#include <stdarg.h>

namespace syntacticanalyzer {

//
// SymbolList
//

	bool SymbolList::appendUnique(Symbol *sym)
	{
		if(!sym)
			return false;

		//check if symbol already exists in our (this) list
		unsigned int i;
		for(i = 0; i < count(); ++i) {
			if(sym == m_symbols[i]) {
				return false;
			}
		}

		append(sym);
		return true;
	}

	unsigned int SymbolList::mergeUnique(const SymbolList &symList)
	{
		unsigned int i;
		unsigned int appendCount = 0;
		for(i = 0; i < symList.count(); ++i) {
			if(appendUnique(symList[i]))
				++appendCount;
		}

		return appendCount;
	}

	bool SymbolList::contains(Symbol *sym)
	{
		unsigned int i;
		if(sym) {
			for(i = 0; i < m_symbols.size(); ++i) {
				Symbol *sym2 = m_symbols[i];
				if(m_symbols[i] == sym)
					return true;
			}
		}

		return false;
	}
}