#pragma once

#include "dependencies/lexer/Lexer.h"
#include "GrammarBuilder.h"
#include "Parser.h"

namespace syntacticanalyzer {
	
	extern "C" GrammarBuilder* createDefaultGrammarBuilder();
	extern "C" Parser* createDefaultParser(Grammar* grammar, Lexer* lexer);

}