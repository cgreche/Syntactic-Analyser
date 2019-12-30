
//Last edit: 17/02/2018 21:59

#include <cstring>
#include <cstdarg>

#include "symbol.h"

namespace syntacticanalyzer {

//
// SymbolList
//

	unsigned int SymbolList::merge(const SymbolList &symList)
	{
		unsigned int i;
		unsigned int appendCount = 0;
		for(i = 0; i < symList.count(); ++i) {
			if(append(symList[i]))
				++appendCount;
		}

		return appendCount;
	}

}