
#include <iostream>
#include <SyntacticAnalyzer.h>

namespace C_language {
	using namespace syntacticanalyzer;
	extern Grammar* C_grammar_create();
}


using namespace C_language;


int main() {
	Grammar* grammar = C_grammar_create();
	unsigned int symbolCount = grammar->symbolCount();
	for (unsigned int i = 0; i < symbolCount; ++i) {
		std::cout << grammar->symbol(i)->name() << "\n";
	}
	return 0;
}