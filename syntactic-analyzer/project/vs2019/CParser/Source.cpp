
#include <SyntacticAnalyzer.h>

namespace C_language {
	using namespace syntacticanalyzer;
	extern Grammar* C_grammar_create();
}


using namespace C_language;


int main() {
	Grammar* grammar = C_grammar_create();
	return 0;
}