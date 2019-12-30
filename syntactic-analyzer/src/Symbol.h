
//Last edit: 17/02/2018 21:59

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <vector>

namespace syntacticanalyzer {

	class Symbol {
	public:
		virtual const char* name() const = 0;
		virtual unsigned int index() const = 0;
		virtual bool isTerminal() const = 0;
		virtual bool isNonTerminal() const = 0;
	};

}

#endif