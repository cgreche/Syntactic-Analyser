//Created in: 29/12/2019 03:19
//Last edit: 29/12/2019 03:19

#pragma once

#include "Symbol.h"

namespace syntacticanalyzer {

	class NonterminalSymbol : public Symbol {
	public:
		virtual void* proList() const = 0;
	};

}
